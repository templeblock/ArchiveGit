/*
// $Header: /PM8/App/PARAREC.CPP 1     3/03/99 6:08p Gbeddow $
//
// Paragraph record routines.
//
// $Log: /PM8/App/PARAREC.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:36   Fred
// Initial revision.
// 
//    Rev 1.2   12 Jul 1996 16:47:08   Jay
// removed old methods
// 
//    Rev 1.1   27 Jun 1996 17:20:36   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:43:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:36   FRED
// Initial revision.
// 
//    Rev 2.11   22 Sep 1995 09:41:30   JAY
// 32-bit change of Change structure.
// 
//    Rev 2.10   14 Sep 1995 09:32:26   JAY
// 32-bit changes
// 
//    Rev 2.9   07 Sep 1995 08:56:32   JAY
// Now adds extra padding on the right of a fill-in field character.
// 
//    Rev 2.8   07 Jul 1995 16:10:44   JAY
// Misc changes.
// 
//    Rev 2.7   01 May 1995 15:24:54   JAY
// Restructured the macro stuff
// 
//    Rev 2.6   01 May 1995 08:42:36   JAY
// Now handles macros correctly in assign().
// 
//    Rev 2.5   28 Apr 1995 17:26:34   JAY
// No longer creates separate macro words.
// 
//    Rev 2.4   19 Apr 1995 12:31:50   JAY
//  
// 
//    Rev 2.3   29 Mar 1995 08:58:36   JAY
// Lots of changes
// 
//    Rev 2.2   15 Mar 1995 10:40:56   JAY
// Support for new text editing
// 
//    Rev 2.1   17 Feb 1995 16:45:32   JAY
// Added insert_character() overload which takes a style.
// 
//    Rev 2.0   07 Feb 1995 15:48:00   JAY
// Initial revision.
// 
//    Rev 1.7   30 Nov 1994 16:35:20   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.6   01 Jun 1994 16:35:22   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.5   23 May 1994 08:18:34   JAY
// Added a destroy() method which destroys the style when the paragraph record
// is permanently deleted from the database.
// 
//    Rev 1.4   16 Mar 1994 16:02:20   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.3   28 Jan 1994 17:10:54   JAY
//  
// 
//    Rev 1.2   06 Jan 1994 16:06:44   JAY
// Added maximum_characters variable.
// 
//    Rev 1.1   05 Jan 1994 08:05:44   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:24:58   JAY
//
*/

#include "stdafx.h"
#include "pmgdb.h"

#include "pararec.h"
#include "macrorec.h"
#include "fontsrv.h"

#include "styleit.h"

/*****************************************************************************/
/*                          Paragraph record                                 */
/*****************************************************************************/

SHORT ParagraphRecord::maximum_characters = 2000;

/*
// The creator for a paragraph record.
*/

ERRORCODE ParagraphRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	if ((*record = (DatabaseRecordPtr)(new ParagraphRecord(number, type, owner, where))) == NULL)
	{
		return ERRORCODE_Memory;
	}
	return ERRORCODE_None;
}

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

VOID ParagraphRecord::destroy(VOID)
{
/* If we have a source record, we need to get rid of it now. */
	style.destroy();
}

/*
// The constructor for a paragraph record.
*/

ParagraphRecord::ParagraphRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where),
				  style((PMGDatabasePtr)owner, FALSE),
				  text(sizeof(CHARACTER)),
				  change(sizeof(STYLE_CHANGE))
{
	record.fHasNewWordData = 1;
	record.m_wFlags = 0;
}

ERRORCODE ParagraphRecord::insert_character(C_INDEX pos, CHARACTER c)
{
	if (text.count() >= maximum_characters)
	{
		return ERRORCODE_Full;
	}
	if (c >= MACRO_CHARACTER)
	{
		record.m_wFlags |= PARAGRAPH_FLAG_HasMacro;
	}
	return text.insert_element(&c, pos);
}

/*
// Insert a character (with style) into the paragraph.
*/

ERRORCODE ParagraphRecord::insert_character(C_INDEX pos, CHARACTER c, TextStyle* pStyle)
{
	CHARACTER_DELTA cdelta;
	CHARACTER_RANGE crange;

	crange.c_start =
		crange.c_end =
		cdelta.c_start = pos;
	cdelta.count = 1;

	ERRORCODE error;

	if ((error = insert_character(pos, c)) == ERRORCODE_None
			&& (error = adjust_style(cdelta)) == ERRORCODE_None)
	{
		change_range_style(crange, pStyle, FALSE);
	}

	return error;
}

/*
// Delete a character.
//
// If pDeletedMacro != NULL, then it must point to an *initialized* BOOL
// variable (typically FALSE). This routine will not set it to FALSE, only
// TRUE as is appropriate.
*/

ERRORCODE ParagraphRecord::delete_character(C_INDEX pos, BOOL* pDeletedMacro /*=NULL*/)
{
	if (pDeletedMacro != NULL)
	{
		CHARACTER* cPtr = get_character(pos);
		if (cPtr != NULL && *cPtr >= MACRO_CHARACTER)
		{
			*pDeletedMacro = TRUE;
		}
	}
	return text.delete_element(pos);
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE ParagraphRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;
   ST_DEV_IO_SIZE record_size_read;
   
	if ((error = device->read_record(&record, sizeof(record), &record_size_read)) == ERRORCODE_None
			&& (error = style.read(device)) == ERRORCODE_None
			&& (error = text.read(device)) == ERRORCODE_None
			&& (error = word.read(device)) == ERRORCODE_None
			&& (error = change.read(device)) == ERRORCODE_None)
	{
		// Do the new text data kludge: If document was store in format from
		// before 1.00.07, the new data was not stored so we set defaults.
		// If the document is from >= 1.00.07, the new data is stored after all
		// other data in the paragraph record.  (MH 3/15/94)
		
		if (record_size_read < sizeof (record))
		{
			record.fHasNewWordData = 0;
		}

#ifdef WIN32
		if (record.fHasNewWordData & 1)
#else
		if (record.fHasNewWordData)
#endif
		{
			error = word.read_new_data(device);
		}
		else
		{
			word.set_new_data_defaults();
		}
	}

	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE ParagraphRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->write_record(&record, sizeof(record))) == ERRORCODE_None
			&& (error = style.write(device)) == ERRORCODE_None
			&& (error = text.write(device)) == ERRORCODE_None
			&& (error = word.write(device)) == ERRORCODE_None
			&&	(error = change.write(device)) == ERRORCODE_None)
		error = word.write_new_data(device);

	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE ParagraphRecord::SizeofData(StorageDevicePtr device)
{
	return device->size_record(sizeof(record))
			+ style.size(device)
			+ text.size(device)
			+ word.size(device)
			+ change.size(device);
}

/*
// Return the index of the word containing a specific character.
// Pass NULL for wp_ret if no return image is desired.
*/

W_INDEX ParagraphRecord::word_of_character(C_INDEX c_index, TEXT_WORD_PTR wp_ret)
{
	W_INDEX w_index;
	TEXT_WORD_PTR wp;

/*
// If it's off the end, just fix it.
// This action should not be assumed, because I may change this to an
// error later.
*/

	if (c_index > text.count())
	{
		od("word_of_character: offset is beyond end of text\r\n");
		return -1;
	}

/*
// Find the offset in the words.
*/

	W_INDEX word_count = word.count();

	for (w_index = 0, wp = (TEXT_WORD_PTR)word.get_element(0);
								w_index < word_count;
								w_index++, wp++)
	{
		C_INDEX c_start = wp->c_start;

		if (c_index <= c_start)
		{
			if (c_index < c_start && w_index != 0)
			{
			/* Character is in previous word. */
				w_index--;
				wp--;
			}
			break;
		}
	}

	if (w_index == word_count)
	{
	/* This is an unavoidable error. */
		od("word_of_character: word array is inconsistent\r\n");
		w_index = -1;
	}
	else
	{
	/* Return an image of this word if requested. */
		if (wp_ret != NULL)
		{
			*wp_ret = *wp;
		}
	}

	return w_index;
}

/*
// Convert a character range to a word range.
*/

SHORT ParagraphRecord::crange_to_wrange(CHARACTER_RANGE crange, WORD_RANGE_PTR wrange)
{
	if (crange.c_start == -1)
	{
		wrange->w_start = -1;
	}
	else
	{
		if ((wrange->w_start = word_of_character(crange.c_start, NULL)) == -1)
		{
			return -1;
		}
	}

	if (crange.c_end == -1)
	{
		wrange->w_end = -1;
	}
	else
	{
		if ((wrange->w_end = word_of_character(crange.c_end, NULL)) == -1)
		{
			return -1;
		}
	}

	return 0;
}

/*
// Read a word entry from a paragraph.
*/

SHORT ParagraphRecord::get_word_image(W_INDEX w_index, TEXT_WORD_PTR wp)
{
	TEXT_WORD_PTR w;

	if ((w = (TEXT_WORD_PTR)word.get_element(w_index)) == NULL)
	{
		return -1;
	}
	else
	{
		*wp = *w;
		return 0;
	}
}

/*
// Size the words in a paragraph.
// Only words which are marked for sizing are sized.
// Passing -1 for w_start means first word.
// Passing -1 for w_end means last word.
*/

ERRORCODE ParagraphRecord::size_words(WORD_RANGE wrange, BOOL force, BOOL fSizeMacros /*=FALSE*/, BOOL* pSized /*=NULL*/)
{
	ASSERT(FALSE);
	return ERRORCODE_None;
}

/*
// Insert a style change.
// This handles face token changes correctly (we own all our changes).
*/

ERRORCODE ParagraphRecord::insert_change(S_INDEX pos, STYLE_CHANGE_PTR s)
{
	if (s->token == TOKEN_Face)
	{
	/*
	// This is a face style change. Increment the face reference count.
	*/
		ERRORCODE error;

		if ((error = ((PMGDatabasePtr)database)->inc_face((DB_RECORD_NUMBER)s->new_value)) != ERRORCODE_None)
		{
			return error;
		}
	}

/*
// We can now insert the style change.
*/
	return change.insert_element(s, pos);
}

/*
// Delete a style change.
// This handles face token changes correctly (we own all our changes).
*/

ERRORCODE ParagraphRecord::delete_change(S_INDEX pos)
{
	ERRORCODE error;
	STYLE_CHANGE sc = *get_style_change(pos);

	if ((error = change.delete_element(pos)) == ERRORCODE_None)
	{
	/*
	// The style change has been deleted.
 	// If it's a face style change, decrement its reference count.
	*/
		if (sc.token == TOKEN_Face)
		{
			error = ((PMGDatabasePtr)database)->free_face((DB_RECORD_NUMBER)sc.new_value, FALSE);
		}
	}
	return error;
}

/*
// Add a style change.
// Style changes at the front of the paragraph get absorbed into the
// paragraph style.
*/

S_INDEX ParagraphRecord::insert_style_change(S_INDEX s_index, STYLE_CHANGE_PTR scp)
{
	if (scp->c_index == 0 && !(scp->token & TOKEN_Temporary))
	{
	/* Change for first character. Apply to base style. */
		style.apply_change(scp);
	}
	else
	{
	/* Add the change for other character. */
		insert_change(s_index++, scp);
	}
	return s_index;
}

/*
// Adjust the style entries following a character array modification.
*/

ERRORCODE ParagraphRecord::adjust_style(CHARACTER_DELTA cdelta)
{
/* Avoid busy work. */

	if (cdelta.count == 0)
	{
		return ERRORCODE_None;
	}

/*
// Adjusting the styles has two phases.
//
// Phase 1: Walk through the style changes and adjust the ones which are
// above or at the character delta. All tokens stop at the delta start
// (bunching up) in order to preserve the style following that point if
// deleting.
*/
	S_INDEX bunch_start = -1;
	SHORT bunch_count = 0;

	TextStyle start_style((PMGDatabasePtr)database), end_style((PMGDatabasePtr)database);

	StyleIterator si(this);

	for (si.style = style, si.scp = get_style_change(0), si.s_index = 0;
							si.s_index < change.count();
							si.s_index++, si.scp++)
	{
	/* See if this style change needs to be adjusted. */

		if (si.scp->c_index >= cdelta.c_start)
		{
			C_INDEX c_new;

			if ((c_new = si.scp->c_index + cdelta.count) < cdelta.c_start)
			{
				c_new = cdelta.c_start;
			}

			si.scp->c_index = c_new;
		}

	/* Check our bunching status. */

		if (si.scp->c_index == cdelta.c_start)
		{
		/* This token has bunched up against the start. Note it. */

			if (bunch_start == -1)
			{
				bunch_start = si.s_index;
				start_style = si.style;
			}
			bunch_count++;
		}
		else
		{
		/* If we just finished a bunched range, set the end style. */

			if (bunch_start != -1 && end_style.get_face() == 0)
			{
				end_style = si.style;
			}
		}

	/* Pick up this style change. */

		si.grab_style_change();
	}

/*
// Phase 2: Handle any tokens which were brought to the delta start point
// and bunched up. Remove any canceling or overriding style changes. A style
// change entry is considered irrelevant if either that style attribute doesn't
// change or the change is not contributed by that particular entry.
*/

	if (bunch_start != -1)
	{
	/* We need to check our bunched range. */

		if (end_style.get_face() == 0)
		{
		/* No style change after the last one apparently. */
			end_style = si.style;
		}

	/* Delete all of these style changes. */

		while (bunch_count-- != 0)
		{
			delete_change(bunch_start);
		}

	/* See if there is any text after our bunch. */

		if (cdelta.c_start != text.count())
		{
			STYLE_CHANGE sc;

			sc.c_index = cdelta.c_start;

		/* Text follows. Add necessary tokens back in. */

			for (sc.token = TOKEN_Face;
									sc.token != LAST_TEXT_TOKEN;
									sc.token = (TEXT_TOKEN)(sc.token+1))
			{
				if (!start_style.compare_styles((TEXT_TOKEN)sc.token, &end_style))
				{
					start_style.build_change(&sc, &end_style);
					bunch_start = insert_style_change(bunch_start, &sc);
				}
			}
		}
	}
	return ERRORCODE_None;
}

/*
// Change the style of a range to what is passed.
//
// Returns:
//			-1 = error
//			 0 = no tokens removed
//			>0 = at least token removed
*/

//#define DEBUG_RCS

SHORT ParagraphRecord::change_range_style(CHARACTER_RANGE crange, TextStylePtr new_style, BOOL temporary)
{
	PMGDatabasePtr pDatabase = (PMGDatabasePtr)database;
	TextStyle LocalStyle(pDatabase, FALSE);
	STYLE_CHANGE sc;
	FLAGS token_modifier = temporary ? TOKEN_Temporary : 0;
	C_INDEX c_start;
	SHORT ret = 0;
	BOOL fLocal = FALSE;

	if (new_style->get_database() != pDatabase)
	{
	/* Bring it local. */
		LocalStyle = *new_style;
		new_style = &LocalStyle;
		fLocal = TRUE;
	}

/*
// We proceed as follows:
// (1) Get the style of the first character in this range, so we can know
// where we're starting.
// (2) Put any tokens necessary at the start of the range to set us up with
// the new style at that location.
// (3) March through the style changes (from start to end). If a style change
// has been superceded, remove it from the array of changes. As we parse
// style changes, build a composite style of what the style used to be so we
// will know what the end style is.
// (4) Put tokens in at the end of the range to insure that the style following
// the range remains intact.
*/

#ifdef DEBUG_RCS
	od("change_range_style %d to %d", crange);
#endif

#ifdef DEBUG_RCS
	od(" (cnt:%d)\r\n", change.count());
#endif

/* Fix the range. */

	if (crange.c_start < 0)
	{
		crange.c_start = 0;
	}

	if (crange.c_end < 0 || crange.c_end >= text.count())
	{
		crange.c_end = text.count()-1;
	}
	crange.c_end++;

/* Find the start at the beginning of the range. */

#ifdef DEBUG_RCS
	od("Find start of range...\r\n");
#endif

/* Compute which character to get the style for. */

	c_start = crange.c_start;

	StyleIterator si(this);

	for (si.style = style, si.s_index = 0, si.scp = get_style_change(0);
							si.s_index < change.count();
							si.s_index++, si.scp++)
	{
		if (si.scp->c_index >= crange.c_start)
		{
			break;
		}

		if (si.scp->token & TOKEN_Temporary)
		{
		/* Always get rid of temporary tokens. */

#ifdef DEBUG_RCS
			od("\tdelete temporary token %d (%x: %ld) @ %d\r\n", si.s_index, si.scp->token, si.scp->new_value, si.scp->c_index);
#endif
			delete_change(si.s_index);
			si.scp = get_style_change(si.s_index--) - 1;
			ret++;
		}
		else
		{
#ifdef DEBUG_RCS
			od("\tgrab change %d (%x: %ld) @ %d\r\n", si.s_index, si.scp->token, si.scp->new_value, si.scp->c_index);
#endif
		/* Get the existing style change. Not overridden anymore. */
			if (si.scp->token & TOKEN_Overridden)
			{
				ret++;
				si.scp->token = (TEXT_TOKEN)(si.scp->token & ~TOKEN_Overridden);
			}
			si.grab_style_change();
		}
	}

/*
// We may need to put tokens in at the front of the range to set us up with
// the new style. Do that now.
*/

	sc.c_index = crange.c_start;

#ifdef DEBUG_RCS
	od("Insert tokens before...\n\r");
#endif
	for (sc.token = TOKEN_Face;
						sc.token < LAST_TEXT_TOKEN;
						sc.token = (TEXT_TOKEN)(sc.token + 1))
	{
	/* If this entry has a defined change... */
		if (new_style->test_style((TEXT_TOKEN)sc.token)
					&& !new_style->compare_styles((TEXT_TOKEN)sc.token, &si.style))
		{
		/* Insert a token for this change. */

			si.style.build_change(&sc, new_style);

		/* Turn it on. */
			sc.token = (TEXT_TOKEN)(sc.token ^token_modifier);
#ifdef DEBUG_RCS
			od("\tInsert token %d (%x: %ld) at %d\r\n", si.s_index, sc.token, sc.new_value, sc.c_index);
#endif
			si.s_index = insert_style_change(si.s_index, &sc);
		/* Turn it back off. */
			sc.token = (TEXT_TOKEN)(sc.token ^token_modifier);
			ret++;
		}
	}

	si.scp = get_style_change(si.s_index);

/*
// Now we have the style at the start of the range.
// Begin processing the tokens.
// Delete any which are represented in the new style.
*/

#ifdef DEBUG_RCS
	od("Parse existing range tokens...\r\n");
#endif

	while (si.s_index < change.count() && si.scp->c_index <= crange.c_end)
	{
	/* Update the "old" style in the iterator. */

		if ((si.scp->token & TOKEN_Temporary)
					|| new_style->test_style((TEXT_TOKEN)si.scp->token))
		{
		/* Component is changing. */
			if (!(si.scp->token & TOKEN_Temporary))
			{
			/*
			// This is not a temporary token.
			// Mark it as not overridden anymore (in case it was).
			*/

				si.scp->token = (TEXT_TOKEN)(si.scp->token & ~TOKEN_Overridden);
				si.grab_style_change();
			}

		/* Remove or otherwise mask it. */

			if ((si.scp->token & TOKEN_Temporary) || !temporary)
 			{
#ifdef DEBUG_RCS
				od("\tDelete token %d (%x: %ld) at %d\r\n", si.s_index, si.scp->token, si.scp->new_value, si.scp->c_index);
#endif
			/*
			// This is a temporary token or we are making permanent changes.
 			// Delete it!
 			*/

				delete_change(si.s_index);

			/* Recalc the pointer in case the array moved. */

				si.scp = get_style_change(si.s_index--) - 1;
			}
			else
			{
#ifdef DEBUG_RCS
				od("\tOverride token %d (%d: %ld) at %d\r\n", si.s_index, si.scp->token, si.scp->new_value, si.scp->c_index);
#endif
			/* This is a token which needs to be temporarily overridden! */

				si.scp->token = (TEXT_TOKEN)(si.scp->token | TOKEN_Overridden);
			}
			ret++;
		}
	/*
	// Next change, please!
	*/

		si.s_index++;
		si.scp++;
	}

/*
// Now, we need to put tokens in at the end of the range to fix up the style
// to be what it was before.
*/

#ifdef DEBUG_RCS
	od("Insert tokens after the range...\r\n");
#endif

	if (crange.c_end != text.count())
	{
		sc.c_index = crange.c_end;

		for (sc.token = TOKEN_Face;
							sc.token < LAST_TEXT_TOKEN;
							sc.token = (TEXT_TOKEN)(sc.token + 1))
		{
		/* If this entry has a defined change in the new style... */

			if (new_style->test_style((TEXT_TOKEN)sc.token)
					&& !new_style->compare_styles((TEXT_TOKEN)sc.token, &si.style))
			{
			/* Insert a token for this change. */

				new_style->build_change(&sc, &si.style);
			/* Turn it on. */
				sc.token = (TEXT_TOKEN)(sc.token ^ token_modifier);
#ifdef DEBUG_RCS
				od("\tInsert token %d (%x: %ld) at %d\r\n", si.s_index, sc.token, sc.new_value, sc.c_index);
#endif
				si.s_index = insert_style_change(si.s_index, &sc);
			/* Turn it back off. */
				sc.token = (TEXT_TOKEN)(sc.token ^ token_modifier);
				ret++;
			}
		}
	}

#ifdef DEBUG_RCS
	od("Done (cnt:%d)...\r\n", change.count());
#endif

/*
// If we needed a local style (to move the style over from another database),
// we needed it to be non-temporary (to do the copy); this means we need to
// destroy() it now to free the face.
*/

	if (fLocal)
	{
		LocalStyle.destroy();
	}

	return ret;
}

/*
// Remove all temporary tokens in a paragraph.
//
// Returns:
//			<0 = ERRORCODE
//			 0 = no tokens removed
//			>0 = at least token removed
*/

SHORT ParagraphRecord::remove_temporary_tokens(VOID)
{
	SHORT ret = 0;

	STYLE_CHANGE_PTR scp;
	S_INDEX s_index;

	for (s_index = 0, scp = get_style_change(0);
								s_index < change.count();
								s_index++, scp++)
	{
		if (scp->token & TOKEN_Temporary)
		{
#ifdef DEBUG_RCS
			printf("Delete temporary token...\n");
#endif
			delete_change(s_index);
			scp = get_style_change(s_index--) - 1;
			ret++;
		}
		else
		{
		/* Not overridden anymore. */
			if (scp->token & TOKEN_Overridden)
			{
				scp->token = (TEXT_TOKEN)(scp->token & ~TOKEN_Overridden);
				ret++;
			}
		}
	}

	return ret;
}

/*
// Make style changes in a paragraph permanent.
//
// Returns:
//			-1 = error
//			 0 = OK
*/

SHORT ParagraphRecord::make_changes_permanent(VOID)
{
	STYLE_CHANGE_PTR scp;
	S_INDEX s_index;

	for (s_index = 0, scp = get_style_change(0);
										s_index < change.count();
										s_index++, scp++)
	{
		if ((scp->token & TOKEN_Overridden) || scp->c_index == 0)
		{
			if (scp->token & TOKEN_Overridden)
			{
#ifdef DEBUG_RCS
				printf("Delete overridden token...\n");
#endif
			}
			else
			{
			/*
			// This is a token at the start of the paragraph.
			// Merge it into the base style.
			*/

#ifdef DEBUG_RCS
				printf("Merging token %d into base style\n", s_index);
#endif
				style.apply_change(scp);
			}
			delete_change(s_index);
			scp = get_style_change(s_index--) - 1;
		}
		else
		{
		/* Not temporary anymore. */
#ifdef DEBUG_RCS
			if (scp->token & TOKEN_Temporary)
			{
				printf("Make token %d permanent at %d\n", s_index, scp->c_index);
			}
#endif
			scp->token = (TEXT_TOKEN)(scp->token & ~TOKEN_Temporary);
		}
	}
	return 0;
}

/*
// Assign method.
*/

ERRORCODE ParagraphRecord::assign(DatabaseRecordRef srecord)
{
	ERRORCODE error;

/* Assign the base record first. */

	if ((error = DatabaseRecord::assign(srecord)) == ERRORCODE_None)
	{
	/*
	// Copy the data over.
	// Care must be taken with sub-records.
	*/
		ParagraphRef pref = (ParagraphRef)srecord;

		record = pref.record;
		style = pref.style;

		if ((error = text.assign(pref.text)) == ERRORCODE_None
				&& (error = word.assign(pref.word)) == ERRORCODE_None
				&& (error = change.assign(pref.change)) == ERRORCODE_None)
		{
		/*
		// If there is a macro character in here, and are going to a new
		// database, we need to transfer it (them).
		*/

			if (HasMacro() && database != pref.database)
			{
				FixupMacros((PMGDatabase*)pref.database);
			}

		/* We need to transfer all the face references to the new database. */

			STYLE_CHANGE_PTR scp;
			SHORT i;

			for (scp = get_style_change(i = 0); i++ < number_of_changes(); scp++)
			{
				if (scp->token == TOKEN_Face)
				{
					DB_RECORD_NUMBER old_face = (DB_RECORD_NUMBER)scp->new_value;

					if (database == pref.database)
					{
						((PMGDatabasePtr)database)->inc_face(old_face);
					}
					else
					{
						CHAR name[80];

					/*
					// Database to database transfer.
				 	// We need to transfer the font record by name.
				 	*/

						((PMGDatabasePtr)pref.database)->font_data_name(old_face, name);
						scp->new_value = (LONG)((PMGDatabasePtr)database)->new_font_record(name);
					}
				}
			}
		}
	}
	return error;
}

/*
// Fixup all references to macros in this paragraph according to the database
// passed.
*/

void ParagraphRecord::FixupMacros(PMGDatabase* pSrcDatabase)
{
	MacroListRecord* pSrcList = pSrcDatabase->GetMacroList();
	if (pSrcList != NULL)
	{
		MacroListRecord* pDstList = ((PMGDatabase*)database)->GetMacroList();
		if (pDstList != NULL)
		{
		/* March through all characters and find the macros. */

			int nChars = number_of_characters();
			ASSERT(nChars != 0);		// If we have a macro, we have a character!
			if (nChars != 0)
			{
				CHARACTER* pChar = get_character(0);
				while (nChars-- > 0)
				{
					CHARACTER c = *pChar;
					if (c >= MACRO_CHARACTER)
					{
						WORD wMacroId = pDstList->ImportMacro(c - MACRO_CHARACTER, pSrcList);
						if (wMacroId == 0)
						{
						/* Assign it something harmless. */
							*pChar = '?';
						}
						else
						{
						/* Assign it the new value. */
							*pChar = wMacroId + MACRO_CHARACTER;
						}
					}
					pChar++;
				}
			}

			pDstList->release();
		}
		pSrcList->release();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Macro support.

BOOL ParagraphRecord::HasMacro(void)
{
	return (record.m_wFlags & PARAGRAPH_FLAG_HasMacro) != 0;
}

void ParagraphRecord::UpdateMacroState(CHARACTER_RANGE* pRange /*=NULL*/)
{
	CHARACTER_RANGE crange;
	int nCharacters = number_of_characters();

/* Set up the range. */

	if (pRange == NULL)
	{
	/* Turn of the macro flag for full update. */
		record.m_wFlags &= ~PARAGRAPH_FLAG_HasMacro;
		crange.c_start = 0;
		crange.c_end = nCharacters;
	}
	else
	{
	/*
	// Partial range assumes flags is already set correctly.
	// Any new macros will just OR with old ones. So we can short-circuit
	// this is we already know we have a macro.
	*/
		if (record.m_wFlags & PARAGRAPH_FLAG_HasMacro)
		{
			return;
		}

		crange = *pRange;
		if (crange.c_start < 0)
		{
			crange.c_start = 0;
		}
		if (crange.c_end > nCharacters)
		{
			crange.c_end = nCharacters;
		}
	}

/*
// Check for a trivial case.
*/

	if (crange.c_start == crange.c_end)
	{
	/* Nothing to do. */
		return;
	}

/*
// Now run through the characters.
// We can stop as soon as we find a macro.
*/

	C_INDEX c_index;
	CHARACTER* cPtr = get_character(c_index = crange.c_start);

	for (; c_index < crange.c_end; c_index++)
	{
		if (*cPtr++ >= MACRO_CHARACTER)
		{
		/* Found one! */
			record.m_wFlags |= PARAGRAPH_FLAG_HasMacro;
			break;
		}
	}
}

/*
// Resize any macros in this paragraph.
*/

BOOL ParagraphRecord::ResizeMacros(WORD_RANGE& wrange, CWordArray* pMacros /*=NULL*/)
{
	BOOL fSized = FALSE;

	if (record.m_wFlags & PARAGRAPH_FLAG_HasMacro)
	{
	/* We need to resize the macros. */
		if (pMacros == NULL)
		{
			size_words(wrange, FALSE, TRUE, &fSized);
		}
		else
		{
		/* See if any macro needs reflowing. */
			int nWords = number_of_words();
			int nMacros = pMacros->GetSize();
			BOOL fFound = FALSE;

			TEXT_WORD_PTR pWord = get_word(0);

			for (int nWord = 0; nWord < nWords; nWord++, pWord++)
			{
				if (pWord->type == WORD_TYPE_macro
					 || pWord->type == WORD_TYPE_solid)
				{
				/* See if any macros are in this word. */
					C_INDEX c_index = pWord->c_start;
					CHARACTER* cp = get_character(c_index);
					C_INDEX c_end = (pWord+1)->c_start-1;
					BOOL fThisWord = FALSE;

					while (c_index++ <= c_end && !fThisWord)
					{
						CHARACTER c = *cp++;

						for (int nMacro = 0; nMacro < nMacros; nMacro++)
						{
							if (pMacros->GetAt(nMacro) == c)
							{
								if (!fFound)
								{
									fFound = TRUE;
									wrange.w_start = nWord;
								}
								wrange.w_end = nWord;
								fThisWord = TRUE;
								break;
							}
						}
					}
				}
			}

			if (fFound)
			{
				size_words(wrange, FALSE, TRUE, &fSized);
			}
		}
	}

	return fSized;
}
