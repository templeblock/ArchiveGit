/*
// Text flowing routines.
//
// $Header: /PM8/App/OLDFLOW.CPP 1     3/03/99 6:08p Gbeddow $
//
// $Log: /PM8/App/OLDFLOW.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:28   Fred
// Initial revision.
// 
//    Rev 1.1   26 Jun 1996 14:21:42   Jay
//  
// 
//    Rev 1.0   24 May 1996 10:14:52   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:44:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:52   FRED
// Initial revision.
// 
//    Rev 2.5   22 Dec 1995 08:00:58   JAY
// Variable used without being initialized!
// 
//    Rev 2.4   03 Aug 1995 07:42:02   JAY
// Special code which prevents line break for empty macros.
// 
//    Rev 2.3   28 Apr 1995 17:26:36   JAY
// No longer creates separate macro words.
// 
//    Rev 2.2   25 Apr 1995 08:51:40   JAY
// Added y-stretch capabilities.
// 
//    Rev 2.1   29 Mar 1995 08:58:46   JAY
// Lots of changes
// 
//    Rev 2.0   07 Feb 1995 15:48:12   JAY
// Initial revision.
// 
//    Rev 1.9   30 Nov 1994 16:35:30   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.8   04 Aug 1994 07:33:42   JAY
// Large model and 32-bit changes
// 
//    Rev 1.7   18 Mar 1994 13:34:06   MACDUFF
// removed debug msgs
// 
//    Rev 1.6   18 Mar 1994 11:06:52   MACDUFF
// Corrected the text height computation (it was wrong when lines
// were of differing heights).
// 
//    Rev 1.5   16 Mar 1994 16:01:40   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.4   09 Mar 1994 09:22:04   MACDUFF
// Added ExtraPixels param to refresh extents
// 
//    Rev 1.3   19 Jan 1994 17:07:00   JAY
//  
// 
//    Rev 1.2   13 Jan 1994 17:00:40   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:06:30   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:12   JAY
// 
*/

#include "stdafx.h"
#if 0
#include <string.h>
#include <stdlib.h>

#include "pmgdb.h"

#include "pararec.h"
#include "framerec.h"
#include "frameobj.h"

#include "textflow.h"

#define DEBUG_ALLx

#ifdef DEBUG_ALL
#define	DEBUG_AW							/* Debug adjust_words */
#define	DEBUG_AL							/* Debug adjust_lines */
#define	DEBUG_SW							/* Debug size_words */
#define	DEBUG_SW_X						/* Extra messages for size_words */
#define	DEBUG_RW							/* Debug rebuild_words */
#define	DEBUG_RL							/* Debug rebuild_lines */
#endif

/*
// Dump a word.
*/

#ifdef DEBUG_DUMP
PRIVATE VOID
dump_word(PARAGRAPH_PTR paragraph, TEXT_WORD_PTR wp)
{
	C_INDEX c_start, c_end;
	CHARACTER_PTR cp;

	if (wp->type == WORD_TYPE_eop)
	{
		printf("<EOP>");
	}
	else if (wp->type == WORD_TYPE_break)
	{
		printf("<Break>");
	}
	else
	{
		printf("'");
		c_start = wp->c_start;
		c_end = (wp+1)->c_start-1;
		for (cp = (CHARACTER_PTR)paragraph->text.data + c_start;
								c_start++ <= c_end; )
		{
			printf("%c", *cp++);
		}
		printf("'");
	}
}
#endif

/*
// add_width_refresh_extent()
//
//		This function adds a refresh extent for a single text line with
// given horizontal bounds.
//
// Arguments:
//		FRAME_PTR frame			frame the text belongs to
//		LINE_PTR line				line the text belongs to
//		PCOORD xmin, xmax			bounds to update, relative to frame left bound
//		REFRESH_TYPE refresh_type		typically REFRESH_ERASE or REFRESH_DRAW
*/

VOID near TextFlow::add_width_refresh_extent(
	FramePtr frame, LINE_PTR lp,
	PCOORD xmin, PCOORD xmax,
	REFRESH_TYPE refresh_type)
{
	if (want_refresh && database->can_refresh())
	{
		PBOX pbox, bound = frame->get_bound();
		PCOORD baseline;
		FLAGS object_flags = frame_object->get_flags();

		if (object_flags & OBJECT_FLAG_xflipped)
		{
			pbox.x0 = bound.x1 - xmax;
			pbox.x1 = bound.x1 - xmin;
		}
		else
		{
			pbox.x0 = bound.x0 + xmin;
			pbox.x1 = bound.x0 + xmax;
		}

		if (pbox.x0 >= pbox.x1)
		{
			/* Null box */
			return;
		}
#if 0
printf("WR: %c%ld %ld%c\n",
 refresh_type == REFRESH_ERASE ? '(' : '[',
 pbox.x0,
 pbox.x1,
 refresh_type == REFRESH_ERASE ? ')' : ']');
#endif
#if 0
	/* Allow for pixels added by hinting and by clear-out */
		pbox.x0 -= 2*redisplay_x_pixel;
		pbox.x1 += 2*redisplay_x_pixel;
#endif

		baseline = lp->baseline;

		pbox.y0 =  baseline - lp->ascend /*- 2*redisplay_y_pixel*/;
		pbox.y1 = baseline + lp->descend /*+ 2*redisplay_y_pixel*/;

		pbox.y0 = __max(pbox.y0, 0);
		pbox.y1 = __min(pbox.y1, bound.y1-bound.y0);

		if (object_flags & OBJECT_FLAG_yflipped)
		{
			PCOORD y0 = pbox.y0;
			pbox.y0 = bound.y1 - pbox.y1;
			pbox.y1 = bound.y1 - y0;
		}
		else
		{
			pbox.y0 += bound.y0;
			pbox.y1 += bound.y0;
		}

RECT rExtraPixels;
SetRect(&rExtraPixels, 2, 2, 2, 2);
		database->do_refresh_notify(&pbox, refresh_type, frame_object, &rExtraPixels);
	}
}

/*
// add_words_refresh_extent()
//
//		Create a refresh extent for a series of consecutive words in
// a single line.  Works only when first_word, last_word point into
// the array of words!!
//
// Arguments:
//		FRAME_PTR frame			frame the text belongs to
//		LINE_PTR line				line the text belongs to
//		TEXT_WORD_PTR first_word		first word of refresh (left bound)
//		TEXT_WORD_PTR last_word			last word of refresh (right bound)
//		REFRESH_TYPE refresh_type		typically REFRESH_ERASE or REFRESH_DRAW
*/

VOID near TextFlow::add_words_refresh_extent(
	FramePtr frame, LINE_PTR lp,
	TEXT_WORD_PTR first_word, TEXT_WORD_PTR last_word,
	REFRESH_TYPE refresh_type)
{
	if (want_refresh && database->can_refresh())
	{
		PCOORD xmin, xmax;

		/* Compute xmin, xmax for these words.  This code allows for the
			possibility that ANY word in the sequence could determine the
			left or right bounds.  This is absolutely necessary!
			*** WARNING: This code relies on the text words being stored
			sequentially! ***
		*/
		{
			TEXT_WORD_PTR wp;

			wp = first_word;

			xmin = wp->x_offset + wp->draw_left;
			xmax = wp->x_offset + wp->draw_width;

			if (first_word < last_word)
			{
				PCOORD x;

				do
				{
					++wp;

					x = wp->x_offset + wp->draw_left;
					if (x < xmin) xmin = x;

					x = wp->x_offset + wp->draw_width;
					if (x > xmax) xmax = x;

				} while (wp < last_word);
			}
		}
		add_width_refresh_extent(frame, lp, xmin, xmax, refresh_type);
	}
}

/*
// Add a refresh extent for this line.
*/

VOID TextFlow::add_line_refresh_extent(FramePtr frame, LINE_PTR lp)
{
	/* Always use REDRAW.  We could get some efficiency by making refresh type
		a parameter to this function, but I tried it and some things looked
		really ugly. (MH 4/23/93) */

	if (want_refresh && database->can_refresh())
	{
		add_width_refresh_extent(frame, lp,
			lp->refresh_xmin, lp->refresh_xmax,
			REFRESH_REDRAW);
	}
}

/*
// Update the word array following a change in the character array.
// Passed is a character delta structure containing the start of the change
// and how many characters were either added (positive delta) or deleted
// (negative delta).
// This ONLY updates the existing words' character indicies. No word boundaries
// are computed (though empty words are deleted).
// A word delta structure is filled out which indicates changes in the word
// array made by this routine.
*/

ERRORCODE TextFlow::adjust_words(DB_RECORD_NUMBER p_record,
					DB_RECORD_NUMBER f_record,
					CHARACTER_DELTA cdelta, WORD_DELTA_PTR wdelta)
{
	ParagraphPtr paragraph;
	W_INDEX w_index;
	TEXT_WORD_PTR wp;
	ERRORCODE error;

	/* Redisplay vars */
	LINE_PTR lp;
	FramePtr frame;
	PCOORD erase_xmin, erase_xmax;
	W_INDEX old_windex;
	L_INDEX l_index;
	BOOL deleted_something;

/*
// Initialize the word delta structure first.
*/

	wdelta->w_start = -1;
	wdelta->count = 0;

/* Make sure we avoid busy work. */

	if (cdelta.count == 0)
	{
		return ERRORCODE_None;
	}

/* Lock the paragraph so we can access it. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		return error;
	}

/* Get the frame object. */

	if ((frame = (FramePtr)database->get_record(f_record, &error, RECORD_TYPE_Frame)) == NULL)
	{
		paragraph->release();
		return error;
	}

/*
// Run through the word array and update all words which need it.
// We must be careful using a pointer into the word array data since any
// deletions can cause the array data to change locations.
*/

	/* Initialize redisplay variables */
	deleted_something = FALSE;
	lp = NULL;
	old_windex = 0;
	l_index = frame->line_of_word(0, NULL);
	lp = (LINE_PTR)frame->line.get_element(l_index);

	for (wp = (TEXT_WORD_PTR)paragraph->word.get_element(0), w_index = 0;
							w_index < paragraph->word.count();
							old_windex++,
 							w_index++, wp++)
	{
		C_INDEX this_start;

		if ((this_start = wp->c_start) >= cdelta.c_start)
		{
		/* This has moved. Move it now. */
		/* If deleting, see if the array is still valid. */

			if ((this_start += cdelta.count) <= cdelta.c_start)
			{
				this_start = cdelta.c_start;
				if (!deleted_something)
				{
					deleted_something = TRUE;

					erase_xmin = wp->x_offset + wp->draw_left;
					erase_xmax = wp->x_offset + wp->draw_width;
				}

			/*
			// Update the word delta.
			// This makes the (valid) assumption that words which get
			// deleted are all contiguous.
			*/

				if (wdelta->w_start == -1)
				{
					wdelta->w_start = w_index;

					/* Start updating w1 (last word to erase) */
					l_index = frame->line_of_word(w_index, NULL);
					lp = (LINE_PTR)frame->line.get_element(l_index);

					erase_xmax = __max(erase_xmax, wp->x_offset + wp->draw_width);
					erase_xmin = __min(erase_xmin, wp->x_offset + wp->draw_left);
				}
				else
				{
				/* The previous word has vanished. Delete it now. */
					if (old_windex > lp->w_end)
					{
						/* That's all the words for this line.  Erase the words */
						add_width_refresh_extent(frame, lp,
									erase_xmin, erase_xmax, REFRESH_ERASE);

						/* Start a new line */
						erase_xmin = wp->x_offset + wp->draw_left;
						erase_xmax = wp->x_offset + wp->draw_width;
						lp++;
					}
					else
					{
						erase_xmax = __max(erase_xmax, wp->x_offset + wp->draw_width);
					}

					wdelta->count--;

				/* Do the actual delete. */

					paragraph->delete_word(--w_index);
					paragraph->modified();
#ifdef DEBUG_AW
					printf("Delete previous word (%d)\n", w_index);
#endif
				/* Recompute 'wp' in case 'word.data' changed. */

					wp = (TEXT_WORD_PTR)paragraph->word.get_element(w_index);
				}
			}

#ifdef DEBUG_AW
			printf("Bumped WORD %d from index %d to %d (", w_index, wp->c_start, this_start);
#endif

		/* Set the new start. */

			wp->c_start = this_start;

#ifdef DEBUG_AW
			dump_word(paragraph, wp);
			printf(")\n");
#endif
		}
		else
		{
			/* Advance the erase anchor point */
			erase_xmin = __min(erase_xmin, wp->x_offset + wp->draw_left);
		}
	}

	if (deleted_something)
	{
		/* Found at least one line to delete.  Delete the last line we found */
		add_width_refresh_extent(frame, lp,
			erase_xmin, erase_xmax, REFRESH_ERASE);
	}

/* Release the objects we got. */

	frame->release();
	paragraph->release();

	return ERRORCODE_None;
}

/*
// Size all words in the paragraph.
// This just calls the paragraph's method.
*/

ERRORCODE TextFlow::size_words(DB_RECORD_NUMBER p_record, WORD_RANGE wrange, BOOL force)
{
	ParagraphPtr paragraph;
	ERRORCODE error;

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) != NULL)
	{
		error = paragraph->size_words(wrange, force);
		paragraph->release();
	}
	return error;
}

/*
// Update the line array following a change in the word array.
// Passed is a word delta structure containing the start of the change
// and how many words were either added (positive delta) or deleted
// (negative delta).
// This ONLY updates the existing lines' word indicies. No line boundaries
// are computed (though empty lines are deleted).
*/

ERRORCODE TextFlow::adjust_lines(DB_RECORD_NUMBER f_record, WORD_DELTA wdelta)
{
	FramePtr frame;
	L_INDEX l_index;
	LINE_PTR lp;
	PCOORD new_top;
	ERRORCODE error;

/* Make sure we avoid busy work. */

	if (wdelta.count == 0)
	{
		return ERRORCODE_None;
	}

/* Lock the frame so we can access it. */

	if ((frame = (FramePtr)database->get_record(f_record, &error, RECORD_TYPE_Frame)) == NULL)
	{
		return error;
	}

/*
// Run through the line array and update all lines which need it.
// We must be careful using a pointer into the line array data since any
// deletions can cause the array data to change locations.
*/

	new_top = -1;

	for (lp = (LINE_PTR)frame->line.get_element(0), l_index = 0;
							l_index < frame->line.count();
 							l_index++, lp++)
	{
		W_INDEX	this_start = lp->w_start,
					this_end = lp->w_end;

		if (this_start >= wdelta.w_start)
		{
			if ((this_start += wdelta.count) < wdelta.w_start)
			{
				this_start = wdelta.w_start;

			/* See if the previous line disappeared. */

				if (l_index > 0)
				{
					LINE_PTR lprev = lp-1;

					if (lprev->w_start == wdelta.w_start)
					{
					/* This line no longer exists. Delete it. */

						new_top = lprev->baseline - lprev->ascend;

						add_line_refresh_extent(frame, lprev);
						frame->delete_line(--l_index);

					/* Recompute 'lp' in case 'line.data' changed. */

						lp = (LINE_PTR)frame->line.get_element(l_index);
					}
					else if (lprev->w_end == wdelta.w_start)
					{
						lprev->w_end--;
					}
				}
			}
		}

		if (this_end >= wdelta.w_start)
		{
			if ((this_end += wdelta.count) < wdelta.w_start)
			{
				this_end = wdelta.w_start;
			}
		}

#ifdef DEBUG_AL
		printf("Bumped LINE %d to index %d, %d\n", l_index, this_start, this_end);
#endif

	/* Set the new start and end. */

		lp->w_start = this_start;
		lp->w_end = this_end;

	/* Lines are moving! */

		if (new_top != -1)
		{
			add_line_refresh_extent(frame, lp);
			lp->baseline = new_top + lp->line_spacing - lp->descend;
			new_top = lp->baseline + lp->descend;
			add_line_refresh_extent(frame, lp);
		}
	}
	frame->release(TRUE);
	return ERRORCODE_None;
}

/*
// Return the type of a character.
*/

WORD_TYPE TextFlow::character_type(CHARACTER c)
{
	if (c == ' ')
	{
		return WORD_TYPE_space;
	}
	else if (c == TAB_CHARACTER)
	{
		return WORD_TYPE_tab;
	}
	else if (c == BREAK_CHARACTER)
	{
		return WORD_TYPE_break;
	}
//	else if (c >= MACRO_CHARACTER)
//	{
//		return WORD_TYPE_macro;
//	}
	else
	{
		return WORD_TYPE_solid;
	}
}

/*
// Reform words in a paragraph between the start and end indicies passed.
// Passing -1 for c_start means first character.
// Passing -1 for c_end means last character.
//
// This routine together with rebuild_lines() make up most of what used
// to be do_text_flow() in Avagio.
//
// A word delta structure is filled out which indicates changes in the word
// array made by this routine.
*/

ERRORCODE TextFlow::rebuild_words(DB_RECORD_NUMBER p_record,
					DB_RECORD_NUMBER f_record,
					CHARACTER_RANGE crange,
					WORD_DELTA_PTR wdelta, WORD_RANGE_PTR wrange)
{
	ParagraphPtr paragraph;
	FramePtr frame;
	W_INDEX w_index, w_start, w_end;
	TEXT_WORD_PTR wp;
	C_INDEX c_index;
	CHARACTER_PTR cp;
	ERRORCODE error;

/*
// Initialize the word delta structure first.
*/

	wrange->w_start =
		wrange->w_end =
		wdelta->w_start = -1;
	wdelta->count = 0;

/* Get the paragraph so we can operate on it. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
	/* Error! */
		return error;
	}

/* See if there is any text. */

	SHORT text_count = paragraph->number_of_characters();

	if (text_count == 0)
	{
	/* No text! No words to form. */
		paragraph->release();
		return ERRORCODE_None;
	}

/* Figure out where to begin forming words. */

	if (crange.c_start < 0)
	{
		crange.c_start = 0;
	}
	if (crange.c_start >= text_count)
	{
		crange.c_start = text_count-1;
	}
	if (crange.c_end < 0 || crange.c_end >= text_count)
	{
		crange.c_end = text_count-1;
	}

	if (crange.c_start > crange.c_end)
	{
	/*
 	// Huh? Bad parameters!
	// We could adjust the parameters to full extent and continue, but then
	// we might not catch a bad programming case.
	// Let's just return an error for now.
 	*/
		od("rebuild_words: bad parameters\r\n");

		paragraph->release();

		return ERRORCODE_BadParameter;
	}

/*
// The first thing we need to do is go through and figure out which words
// are going to be rebuilt. The first word to be rebuilt is the word containing
// the start change (or the previous one if it's the first character of that
// word). The c_index where word forming starts is the first character of
// this word. The last word to be rebuilt is the word containing the end
// change. The c_index where word forming stops is the last character of this
// word. These words will be deleted and rebuilt from the c_index range
// generated.
*/

/*
// A paragraph should always have at least one word (the eop word).
// OK. Here's the first problem: what happens if c_start > word(eop).c_start?
// This means that the character indicies are inconsistent. This shouldn't
// happen, so let's see if we can detect it and flag an error.
*/

/*
// Look for the word containing the start offset.
*/

	SHORT word_count = paragraph->number_of_words();

	w_start = w_end = word_count;

	for (w_index = 0, wp = (TEXT_WORD_PTR)paragraph->word.get_element(0);
						w_index < word_count;
						wp++, w_index++)
	{
		if (wp->c_start >= crange.c_start)
		{
			w_start = w_index-1;
			break;
		}
	}

	if (w_start == word_count)
	{
	/* Couldn't find the c_index in our word array. Inconsistent. */
		paragraph->release();
		od("rebuild_words: start index inconsistent\r\n");
		return ERRORCODE_BadParameter;
	}
	else if (w_start < 0)
	{
	/* Means c_start is in first word. */
		w_start = 0;
	}

#ifdef DEBUG_RW
	printf("Found start offset %d in word %d (", crange.c_start, w_start);
	dump_word(paragraph, (TEXT_WORD_PTR)paragraph->word.data + w_start);
	printf(")\n");
#endif

/* Get the first index in this word. */

	wrange->w_start =
		wrange->w_end =
		wdelta->w_start = w_start;

	if ((c_index = paragraph->get_word(w_start)->c_start) < crange.c_start)
	{
	/* Move back to start of word. */
		crange.c_start = c_index;
		wdelta->w_start++;			/* First word stays put. */
	}

#ifdef DEBUG_RW
	printf("Start offset moves to %d\n", crange.c_start);
#endif

/*
// Look for the word containing the end offset.
*/

	for (; w_index < word_count; wp++, w_index++)
	{
		if (wp->c_start >= crange.c_end)
		{
			w_end = w_index;
			break;
		}
	}

	if (w_end == word_count)
	{
	/* Couldn't find the c_index in our word array. Inconsistent. */
		paragraph->release();

		od("rebuild_words: end index inconsistent\r\n");

		return ERRORCODE_IntError;
	}
	else if (w_end == word_count-1)
	{
	/* We ended up on the eop word. Move off of it. */
		w_end--;
	}

#ifdef DEBUG_RW
	printf("Found end offset %d in word %d (", crange.c_end, w_end);
	dump_word(paragraph, (TEXT_WORD_PTR)paragraph->word.data + w_end);
	printf(")\n");
#endif

/* Get the first index of the next word. */

	if ((c_index = paragraph->get_word(w_end+1)->c_start-1) > crange.c_end)
	{
	/* Move forward to end of word. */
		crange.c_end = c_index;
	}

#ifdef DEBUG_RW
	printf("End offset moves to %d\n", crange.c_end);
#endif

/*
// Delete the existing range if we have one.
// Perhaps this should be optimized someday to do all at once!
*/
	if (w_end >= w_start)
	{
		/* Redisplay variables */
		BOOL new_line = TRUE;
		L_INDEX l_index;
		LINE_PTR lp;
		TEXT_WORD_PTR wp0;
		PCOORD erase_xmin, erase_xmax;

		if ((frame = (FramePtr)database->get_record(f_record, &error, RECORD_TYPE_Frame)) == NULL)
		{
			paragraph->release();
			return error;
		}

		/* Get line pointer for redisplay purposes */

		l_index = frame->line_of_word(w_end, NULL);

		lp = frame->get_line(l_index);

		erase_xmin = 0x7fffffff;
		erase_xmax = -erase_xmin;

		wdelta->count = -(w_end - w_start + 1);
		while (w_end >= w_start)
		{
#ifdef DEBUG_RW
			printf("Delete word %d\n", w_end);
#endif
			wp0 = paragraph->get_word(w_end);
			if (!new_line && w_end < lp->w_start)
			{
				new_line = TRUE;
				add_width_refresh_extent(frame, lp,
					erase_xmin, erase_xmax, REFRESH_ERASE);
				lp--;
			}
			if (new_line)
			{
				new_line = FALSE;

				erase_xmin = wp0->x_offset + wp0->draw_left;
				erase_xmax = wp0->x_offset + wp0->draw_width;
			}
			else
			{
				erase_xmin = __min(erase_xmin, wp0->x_offset + wp0->draw_left);
				erase_xmax = __max(erase_xmax, wp0->x_offset + wp0->draw_width);
			}

			paragraph->delete_word(w_end--);
		}
		add_width_refresh_extent(frame, lp,
				erase_xmin, erase_xmax, REFRESH_ERASE);
		frame->release();
	}

/*
// Start forming new words.
*/

	for (w_index = w_start, c_index = crange.c_start,
 					cp = paragraph->get_character(c_index);
							c_index <= crange.c_end; )
	{
		TEXT_WORD word;

	/* Start this word. */

#ifdef DEBUG_RW
		printf(*cp < ' ' ? "[%d]" : "[%c]", *cp);
#endif

		word.type = character_type(*cp++);
		word.flags = WORD_FLAG_needs_sizing;
		word.c_start = c_index++;
		word.x_offset = -1;
		word.width = 0;

	/* Accumulate the rest of the word. */

		if (word.type == WORD_TYPE_solid)
		{
		/* Search for the next word start. */

			while (c_index <= crange.c_end && character_type(*cp) == word.type)
			{
#ifdef DEBUG_RW
				printf(*cp < ' ' ? "(%d)" : "(%c)", *cp);
#endif

				cp++;
				c_index++;
			}
		}

	/* This is a whole new word. */

#ifdef DEBUG_RW
		printf(" ADD word %d, type %d, c (%d to %d) \n", w_index, word.type, word.c_start, c_index-1);
#endif
		paragraph->insert_word(w_index++, &word);

	/* Another delta in the word array. */

		wdelta->count++;
		wrange->w_end++;
	}

/* Release the paragraph. */

	paragraph->release(TRUE);

	return ERRORCODE_None;
}

/*
// Vertically position the lines in a frame.
//
//	4/22/93 (MH) Also update refresh bounds for frame object.
*/

VOID TextFlow::position_lines(FramePtr frame, VERT_ALIGN_TYPE align_type)
{
	PCOORD y_offset;
	L_INDEX l_index;
	LINE_PTR lp;
	PBOX refresh_offsets;
	PBOX bound = frame->get_bound();
	PCOORD baseline;

/*
// Compute the height of all the lines so we can know how to align them.
*/

	y_offset = 0;
	baseline = 0;

	SHORT line_count = frame->number_of_lines();

	if (line_count == 0)
	{
		refresh_offsets.x0 = 0;
		refresh_offsets.x1 = 0;
	}
	else
	{
		PCOORD xmin, xmax;
		xmin = 0x7fffffff;
		xmax = -xmin;

		for (lp = frame->get_line(0), l_index = 0; l_index < line_count; lp++, l_index++)
		{
			xmin = __min(xmin, lp->refresh_xmin);
			xmax = __max(xmax, lp->refresh_xmax);

			if (l_index == 0)
			{
				baseline = y_offset + lp->ascend;
			}
			else
			{
				baseline = y_offset + lp->line_spacing - lp->descend;
			}
			y_offset = baseline + lp->descend;
		}
		refresh_offsets.x0 = xmin;
		refresh_offsets.x1 = xmax;
	}

/*
// Compute the offset value from the height of the text and the frame height.
*/

	if ((y_offset = bound.y1 - bound.y0 - y_offset) > 0)
	{
		switch (align_type)
		{
			case ALIGN_top:
			{
				y_offset = 0;
				break;
			}
			case ALIGN_middle:
			{
				y_offset /= 2;
				break;
			}
			case ALIGN_bottom:
			default:
			{
				break;
			}
		}
	}
	else
	{
	/* Text too big for frame. Top align it. */
		y_offset = 0;
	}

	refresh_offsets.y0 = y_offset;

/*
// We have the amount to offset. Do the offset now.
*/

	for (lp = frame->get_line(0), l_index = 0; l_index < line_count; lp++, l_index++)
	{
		if (l_index == 0)
		{
			y_offset += lp->ascend;
		}
		else
		{
			y_offset += lp->line_spacing - lp->descend;
		}

		if (y_offset != lp->baseline)
		{
			if (lp->baseline != -1)
			{
				add_line_refresh_extent(frame, lp);
			}
			lp->baseline = y_offset;
			add_line_refresh_extent(frame, lp);
		}
		y_offset += lp->descend;
	}

	refresh_offsets.y1 = __min(y_offset, bound.y1 - bound.y0);

	frame->set_refresh_offsets(refresh_offsets);
}

/*
// Rebuild the lines in a frame.
// This routine together with rebuild_words() make up most of what used
// to be do_text_flow() in Avagio.
*/

ERRORCODE TextFlow::rebuild_lines(FrameObjectPtr object, WORD_RANGE wrange)
{
	ERRORCODE error;

	DB_RECORD_NUMBER f_record = object->get_frame_record();
	FramePtr frame;
	DB_RECORD_NUMBER p_record;
	ParagraphPtr paragraph;

	W_INDEX w_index;
	TEXT_WORD_PTR wp;
	PCOORD line_top;
	LINE_PTR lp;
	L_INDEX l_index;
	PCOORD frame_extent, frame_left, tab_size;
	PCOORD baseline;
	BOOL use_white_space;
	FLAGS frame_flags;

	frame_object = object;

/*
// Text is flowed by placing text words into lines within a frame.
// Words are flowed until the end is reached and then until words stop moving.
*/

	if ((frame = (FramePtr)database->get_record(f_record, &error, RECORD_TYPE_Frame)) == NULL)
	{
		return error;
	}

	frame_flags = frame->get_flags();

/*
// See if we need to do any stretching.
*/
	
	if (frame_flags & (FRAME_FLAG_stretch_frame | FRAME_FLAG_stretch_text))
	{
		if (frame_flags & FRAME_FLAG_stretch_frame)
		{
			SHORT error;

		/* Stretch the frame before flowing. */

			if ((error = object->stretch_frame()) != 0)
			{
				frame->release();
				if (error > 0)
				{
					error = ERRORCODE_None;
				}
				return (ERRORCODE)error;
			}

		/* Frame did not change size. Flow normally. */
		}
#if 0
		else
#else
	/* Always check (or check, too). */
		if (frame_flags & FRAME_FLAG_stretch_text)
#endif
		{
		/* Stretch the text before flowing. */

			if ((error = frame->stretch_text()) != ERRORCODE_None)
			{
				frame->release();
				return error;
			}

		/* Flow all the text. */

			wrange.w_start = wrange.w_end = -1;
		}
	}

	p_record = frame->get_paragraph();

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		frame->release();
		return error;
	}

/*
// Ascertain the true start and end of the text flow request.
*/

	SHORT word_count = paragraph->number_of_words();

	if (wrange.w_start < 0)
	{
		wrange.w_start = 0;
	}
	if (wrange.w_end < 0)
	{
		wrange.w_end = word_count-1;
	}

	if (wrange.w_start > wrange.w_end || wrange.w_end >= word_count)
	{
	/* Error of a parameter nature. */

		paragraph->release();
		frame->release();

		return ERRORCODE_BadParameter;
	}

/*
// Find the line where the desired word starts.
// If the word hasn't been flowed yet, it may be "between" existing lines.
// If a word is not in a line, we'll shove it into the line just following it.
// This should always be possible, because the EOP word should always be
// flowed and in a line (and no word should come after it).
//
// We can do this really easily by just checking the last word in the line.
// If we are before that line, we have our line.
*/

	SHORT line_count = frame->number_of_lines();

	for (lp = frame->get_line(0), l_index = 0;
									l_index < line_count;
									l_index++, lp++)
	{
		/* MH 4/25/93 this was "if (lp->w_end >= wrange.w_start)" */
		if (lp->w_end + 1 >= wrange.w_start)
		{
			break;
		}
	}

/*
// Adjust w_start to the start of the line or our word, whichever is first.
*/

	if (wrange.w_start > lp->w_start)
	{
		wrange.w_start = lp->w_start;
	}

#ifdef DEBUG_RL
	printf("Flow words from %d to %d @ line %d\n", wrange.w_start, wrange.w_end, l_index);
#endif

/*
// Now, we have the line (l_index) and the first word to flow (w_start) into
// it. Let's do some flowin'!
*/

/* Compute the flowable extent and the size of a tab. */

	TextStyleRef style = paragraph->get_style();
	PBOX bound = frame->get_bound();

	frame_left = style.get_left_margin();
	frame_extent = bound.x1 - bound.x0
				- (style.get_left_margin() + style.get_right_margin());
	use_white_space = frame_flags & FRAME_FLAG_use_white_space;

	if ((tab_size = frame_extent / 16) == 0)
	{
		tab_size = 1;
	}

/* Compute the current top of line. */

	baseline = lp->baseline;
	line_top = baseline - lp->ascend;
#ifdef DEBUG_RL
	printf("current baseline: %ld, ascend: %ld, line_top:%ld\n",
						baseline, lp->ascend, line_top);
#endif

	for (w_index = wrange.w_start, wp = paragraph->get_word(w_index);; )
	{
		PCOORD extent;					/* Horizontal extent. */
		PCOORD x_offset;				/* Horizontal position. */
		PCOORD solid_x_offset;
		LINE old_line;
		BOOL line_changed;

#ifdef DEBUG_RL
		printf("(flow next line)\n");
#endif

	/* Remember the old line so we can tell if it moved. */

		old_line = *lp;

	/* Start a new line. */

		lp->w_start = w_index;

	/* Initialize the horizontal extent. */

		extent = frame_extent;

	/* Plug in parameters for this line and update line_top. */

		lp->ascend = lp->descend = lp->line_spacing = 0;

		baseline = line_top;

	/* Start us at the left margin. */

		x_offset = 0;
		solid_x_offset = 0;

	/* Flow words into this line until done (possibly passing w_end). */

		while (w_index < word_count)
		{
			FLAGS flags = wp->flags;

		/* Calculate tab width if necessary. */

			if (wp->type == WORD_TYPE_tab)
			{
				wp->width = ((x_offset + tab_size)/tab_size)*tab_size - x_offset;
			}

		/* See if this word will fit in the row. */

#ifdef DEBUG_RL
			printf("flow word ");
			dump_word(paragraph, wp);
			printf("...");
#endif

			if (flags & WORD_FLAG_override)
			{
			/* Something is overridden. */

				wp->flags &= ~WORD_FLAG_override;

				if (flags & WORD_FLAG_override_flow)
				{
				/*
 				// A flow word!
				// Note that the flags have been cleared, so we won't keep
				// hitting this.
				*/
					break;
				}
			}
			else if (x_offset + wp->width > extent		/* doesn't fit */
						&& w_index != lp->w_start				/* not first word */
						&& !(wp->type == WORD_TYPE_space
									&& ((wp-1)->type == WORD_TYPE_solid
										 || (wp-1)->type == WORD_TYPE_macro))
						&& wp->width != 0)						/* real word */
			{
#ifdef DEBUG_RL
				printf(" Move word to next line\n");
#endif
				break;
			}

		/* Update the horizontal offsets. */

			x_offset += wp->width;
			if (use_white_space
				 || wp->type == WORD_TYPE_solid
				 || wp->type == WORD_TYPE_macro)
			{
				solid_x_offset = x_offset;
			}

			if (wp->ascend > lp->ascend)
			{
#ifdef DEBUG_RL
				printf("Expand ascend from %ld to %ld\n", lp->ascend, wp->ascend);
#endif
				baseline -= lp->ascend;
				baseline += (lp->ascend = wp->ascend);
#ifdef DEBUG_RL
				printf("Baseline moves to %ld\n", baseline);
#endif
			}

			if (wp->descend > lp->descend)
			{
#ifdef DEBUG_RL
				printf("Expand descend from %ld to %ld\n", lp->descend, wp->descend);
#endif
				lp->descend = wp->descend;
			}

			if (wp->line_spacing > lp->line_spacing)
			{
				lp->line_spacing = wp->line_spacing;
			}

		/* Add this word to the line. */

			lp->w_end = w_index++;

#ifdef DEBUG_RL
			printf(" Add word to line (x:%ld, w:%ld).\n", x_offset-wp->width, wp->width);
#endif

		/* If we just placed a break word, this line is done. */

			if (wp++->type == WORD_TYPE_break)
			{
			/*
			// If the previous word:
			// (a) was a "macro" word,
			// (b) started at the beginning of the line, and
			// (c) had zero width,
			// don't flow the break to the next line.
			*/
				BOOL fBreak = TRUE;
				if (w_index >= 2 && x_offset == 0)
				{
					TEXT_WORD_PTR wprev = wp-2;

					/* I make an assumption - x_offset == 0 --> width == 0. */
//					if (wprev->width == 0)
					{
						if (*(paragraph->get_character(wprev->c_start)) >= MACRO_CHARACTER)
						{
							fBreak = FALSE;
						}
					}
				}

				if (fBreak)
				{
#ifdef DEBUG_RL
					printf("(Breaking on BREAK word)\n");
#endif
				/* We need to break for this line. */
					break;
				}
			}
		}

#ifdef DEBUG_RL
		printf("[ line %d base:%ld, asc:%ld, dsc:%ld, w: %d to %d ]\n",
							l_index,
 							baseline, lp->ascend, lp->descend,
							lp->w_start, lp->w_end);
#endif
	/*
 	// We now know all the words in this line. Handle the placement.
 	*/

	/* First calculate the alignment offset. */

		if ((x_offset = extent - solid_x_offset) > 0)
		{
		/* Some space to distribute. */
			switch (style.get_line_alignment())
			{
				case ALIGN_center:
				{
					x_offset /= 2;
					break;
				}
				case ALIGN_left:
				{
					x_offset = 0;
					break;
				}
				default:
				{
					break;
				}
			}
		}
		else
		{
			x_offset = 0;
		}

	/* Now set the positions of the words in this line. */

		line_changed = FALSE;				/* Cross your fingers. */
		x_offset += style.get_left_margin();

		{
			W_INDEX w_index = lp->w_start, w_end = lp->w_end;
			TEXT_WORD_PTR wp = paragraph->get_word(w_index);

			/* Redisplay variables */
			TEXT_WORD_PTR first_word_change = NULL;
			TEXT_WORD_PTR last_word_change = NULL;
			PCOORD erase_xmin, erase_xmax;
			BOOL found_erase = FALSE;

			lp->refresh_xmin = 0x7fffffff;
			lp->refresh_xmax = -lp->refresh_xmin;

			while (w_index <= w_end)
			{
				if (x_offset != wp->x_offset || (wp->flags & WORD_FLAG_changed_size))
				{
				/* A word moved. */
#ifdef DEBUG_RL
					printf("word %d changed from %ld to %ld\n",
									w_index, wp->x_offset, x_offset);
#endif
					if (first_word_change == NULL)
					{
  						first_word_change = wp;
					}
					if (!found_erase && wp->x_offset != -1 &&
						w_index >= old_line.w_start && w_index <= old_line.w_end)
					{
						erase_xmin = wp->x_offset + wp->draw_left;
						erase_xmax = wp->x_offset + wp->draw_width;
  						found_erase = TRUE;
					}
					last_word_change = wp;
					if (wp->x_offset != -1 &&
						 w_index >= old_line.w_start && w_index <= old_line.w_end)
					{
						erase_xmin = __min(erase_xmin, wp->x_offset + wp->draw_left);
						erase_xmax = __max(erase_xmax, wp->x_offset + wp->draw_width);
					}


					wp->x_offset = x_offset;
//remove this to do word-wise refresh; restore it to do full line refresh
//					line_changed = TRUE;
				}
				wp->flags &= ~WORD_FLAG_changed_size;

				lp->refresh_xmin = __min(lp->refresh_xmin, wp->x_offset + wp->draw_left);
				lp->refresh_xmax = __max(lp->refresh_xmax, wp->x_offset + wp->draw_width);

				x_offset += wp++->width;
				w_index++;
			}

			if (first_word_change != NULL && old_line.baseline != -1)
			{
				/* Add a DRAW for every word that moved; erase moved words from
					their old positions. */

  				add_words_refresh_extent(frame, &old_line,
								first_word_change, last_word_change, REFRESH_DRAW);
				if (found_erase)
				{
	  				add_width_refresh_extent(frame, &old_line,
									erase_xmin, erase_xmax, REFRESH_ERASE);
				}
			}
		}

	/* See if the line changed. */

		if (!line_changed)
		{
		/* Words stayed the same. See if the line is different somehow. */
		/* Compare using the OLD_LINE structure. This allows us to ignore
			the changes in refresh_xmin, refresh_xmax. */

			line_changed = (memcmp(&old_line, lp, sizeof(OLD_LINE)) != 0);
			if (line_changed)
			{
				lp->baseline = -1;
			}
		}

		if (line_changed)
		{
#ifdef DEBUG_RL
			printf("Line %d changed\n", l_index);
			printf("old [%d to %d] vs. new [%d to %d]\n",
								old_line.w_start, old_line.w_end,
								lp->w_start, lp->w_end);
#endif
			if (old_line.baseline != -1)
			{
				add_line_refresh_extent(frame, &old_line);
			}
		}
		else
		{
#ifdef DEBUG_RL
			printf("Line %d stayed the same\n", l_index);
#endif
		/* We can end now! */
			if (w_index > wrange.w_end)
			{
				break;
			}
		}

	/* See if we're done. */

		if (w_index >= word_count)
		{
		/* Run out of words. Finished. */
			break;
		}

	/*
 	// Move to a new line.
	// Add one if there are no more left.
 	*/

		line_top = baseline + lp->descend;

		if (++l_index == frame->number_of_lines())
		{
		/* Create a new dummy line. */
			LINE line;
#ifdef DEBUG_RL
			printf("(Create a new line)\n");
#endif

			line.w_start = line.w_end = 0;			/* In case. */
			line.baseline = -1;
			frame->insert_line(l_index, &line);
			lp = frame->get_line(l_index);
		}
		else
		{
		/* Add a refresh extent for the old line in case it's changing. */
#ifdef DEBUG_RL
			printf("(Move onto existing line)\n");
#endif
			lp++;
		}
	}

#ifdef DEBUG_RL
	printf("Done (final l_index:%d, w_index:%d)...\n", l_index, w_index);
#endif

/* If we flowed all words, delete any trailing lines. */

	if (w_index == word_count)
	{
		if (++l_index < frame->number_of_lines())
		{
			while (l_index < frame->number_of_lines())
			{
#ifdef DEBUG_RL
				printf("{Delete line}");
#endif
				add_line_refresh_extent(frame, frame->get_line(l_index));
				frame->delete_line(l_index);
			}
#ifdef DEBUG_RL
			printf("\n");
#endif
		}
	}

	if (frame_flags & FRAME_FLAG_ystretch_frame)
	{
		position_lines(frame, style.get_vertical_alignment());
		object->ystretch_frame();
	}

	position_lines(frame, style.get_vertical_alignment());

/*
// Release our objects.
*/

	paragraph->release(TRUE);
	frame->release(TRUE);

	return ERRORCODE_None;
}

/*
// Update all the lines in a frame.
*/

ERRORCODE TextFlow::refresh_frame(DB_RECORD_NUMBER f_record, CHARACTER_RANGE crange)
{
	ERRORCODE error;
	DB_RECORD_NUMBER p_record;
	WORD_RANGE wrange;
	FramePtr frame;
	ParagraphPtr paragraph;
	LINE line;
	PBOX pbox;

/* Get the frame. */

	if ((frame = (FramePtr)database->get_record(f_record, &error, RECORD_TYPE_Frame)) == NULL)
	{
		return error;
	}

	p_record = frame->get_paragraph();

/* Get the paragraph of the frame. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		frame->release();
		return error;
	}

/* Fix the character range if special values were used. */

	if (crange.c_start < 0)
	{
		crange.c_start = 0;
	}
	if (crange.c_end < 0)
	{
		crange.c_end = paragraph->number_of_characters();
	}

/* Get the word range. */

	paragraph->crange_to_wrange(crange, &wrange);
	paragraph->release();

/* Get the top (and possibly bottom) line.*/

	frame->line_of_word(wrange.w_start, &line);

	pbox.y0 = line.baseline - line.ascend;

/* Now see if the words differ. If not, we must be on the same line. */

	if (wrange.w_end != wrange.w_start)
	{
	/* Get the bottom line. */
		frame->line_of_word(wrange.w_end, &line);
	}

	pbox.y1 = line.baseline + line.descend;

/* Get the frame so we can get the bound. */

	PBOX bound = frame->get_bound();
	FLAGS object_flags = frame_object->get_flags();

	pbox.x0 = bound.x0;
	pbox.x1 = bound.x1;

	if (object_flags & OBJECT_FLAG_yflipped)
	{
		PCOORD y0 = pbox.y0;
		pbox.y0 = bound.y1 - pbox.y1;
		pbox.y1 = bound.y1 - y0;
	}
	else
	{
		pbox.y0 += bound.y0;
		pbox.y1 += bound.y0;
	}

	frame->release();

/* Do the actual refresh add. */

	database->do_refresh_notify(&pbox, REFRESH_ALL, NULL);

	return ERRORCODE_None;
}

#endif
