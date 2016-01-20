/*
// $Header: /PM8/App/STYLE.CPP 1     3/03/99 6:12p Gbeddow $
//
// Implementation for text style objects.
//
// $Log: /PM8/App/STYLE.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:38   Fred
// Initial revision.
// 
//    Rev 1.2   12 Jul 1996 16:47:08   Jay
// removed old methods
// 
//    Rev 1.1   27 Jun 1996 17:20:38   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:48:14   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:22   FRED
// Initial revision.
// 
//    Rev 2.4   22 Sep 1995 09:41:30   JAY
// 32-bit change of Change structure.
// 
//    Rev 2.3   07 Jul 1995 16:10:52   JAY
// Misc changes.
// 
//    Rev 2.2   05 Jun 1995 11:31:08   JAY
// Got rid of DGROUP objects.
// 
//    Rev 2.1   15 Mar 1995 10:40:56   JAY
// Support for new text editing
// 
//    Rev 2.0   07 Feb 1995 15:48:08   JAY
// Initial revision.
// 
//    Rev 1.4   30 Nov 1994 16:38:00   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   04 Aug 1994 07:34:02   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   16 Mar 1994 16:01:38   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.1   05 Jan 1994 08:08:08   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:28   JAY
// 
*/

#include "stdafx.h"
#include "pmgdb.h"

#include "stylehlp.h"
#include "fontsrv.h"

#include <string.h>

/*
// Our little helpers.
*/

PRIVATE StyleFaceHelper face_helper;
PRIVATE StyleFontStyleHelper font_style_helper;
PRIVATE StyleSizeHelper size_helper;
PRIVATE StylePatternHelper pattern_helper;
PRIVATE StyleOutlineHelper outline_helper;
PRIVATE StyleBackgroundHelper background_helper;
PRIVATE StyleShadowHelper shadow_helper;
PRIVATE StyleColorHelper color_helper;
PRIVATE StyleBoldHelper bold_helper;
PRIVATE StyleItalicHelper italic_helper;
PRIVATE StyleUnderlineHelper underline_helper;
PRIVATE StyleUnderlineColorHelper underline_color_helper;

/*
// The helper lineup.
// There needs to be a helper for each token, in the right order.
*/

PRIVATE StyleHelper * near
helper[] =
{
	&face_helper,
	&font_style_helper,
	&size_helper,
	&pattern_helper,
	&outline_helper,
	&background_helper,
	&shadow_helper,
	&color_helper,
	&bold_helper,
	&italic_helper,
	&underline_helper,
	&underline_color_helper
};

/*
// Standard constructor for a style.
*/

TextStyle::TextStyle(PMGDatabasePtr owner, BOOL temp) : temporary(temp), database(owner)
{
	memset(&record, 0, sizeof(record));
	m_nUnderlineThickness = 1;
	m_nUnderlinePosition = 0;
}

/*
// Copy constructor.
*/

TextStyle::TextStyle(const TextStyle far &s)
{
//	memset(&record, 0, sizeof(record));

	database = NULL;
	temporary = TRUE;						// This should cover most cases.

	*this = s;                          
}

/*
// Our destructor.
*/

TextStyle::~TextStyle()
{
	if (temporary)
	{
		destroy();
	}
}

/*
// The style structure is about to be destroyed.
// Clean up.
*/

VOID TextStyle::destroy(VOID)
{
	set_face(0);
}

/*
// The assignment operator.
// This is necessary to handle face locking.
//
// Assignment of a style with a different database will operate correctly.
// (But the style will be owned by the new database afterwards).
*/

TextStyleRef TextStyle::operator = (const TextStyle far &s)
{
/* Free any old face. */

	if (database != NULL)
	{
		database->free_face(record.face, temporary);
	}

/* Copy the data. */

	memcpy(&record, &s.record, sizeof(record));
	m_nUnderlinePosition = s.m_nUnderlinePosition;
	m_nUnderlineThickness = s.m_nUnderlineThickness;

/* Do we copy the database? I think so. */

	if (temporary || database == NULL)
	{
	/* Temporary or uninitialized styles always assume the database assigned. */
		database = s.database;
	}

	if (database == s.database)
	{
	/* Increment the new face ref count. */
		database->inc_face(record.face, temporary);
	}
	else
	{
		CHAR name[80];

	/*
	// Database to database transfer.
 	// We need to transfer the font record by name.
 	*/

		s.database->font_data_name(record.face, name);
		record.face = database->new_font_record(name, temporary);
	}

	return *this;
}

/*
// The equality operator.
*/

BOOL TextStyle::operator == (const TextStyle far &s)
{
	return memcmp(&record, &s.record, sizeof(record)) == 0
							&& database == s.database;
}

/*
// Set the default for this text style.
*/

VOID TextStyle::set_default(VOID)
{
	ASSERT(FALSE);
//	database->get_font_server()->set_default(this);
}

/*
// Update font metrics for this style.
*/

VOID TextStyle::update_font_metrics(VOID)
{
	ASSERT(FALSE);
//	database->get_font_server()->font_metrics(this);
}

/*
// Return the masked token for value passed.
*/

TEXT_TOKEN TextStyle::valid_token(TEXT_TOKEN token)
{
	token = (TEXT_TOKEN)(token & TOKEN_Mask);
	if (token >= 0 && token < LAST_TEXT_TOKEN)
	{
		return token;
	}
	return TOKEN_Illegal;
}

/*
// Apply a style change.
*/

ERRORCODE TextStyle::apply_change(STYLE_CHANGE_PTR scp, BOOL fix_style)
{
	TEXT_TOKEN token;

/* Pick up the style change encoded in 'scp'. */

	if ((token = valid_token((TEXT_TOKEN)scp->token)) != TOKEN_Illegal)
	{
		if (!(scp->token & TOKEN_Overridden))
		{
			helper[token]->apply_change(this, scp);
			if (fix_style)
			{
				update_font_metrics();
			}
		}
		return ERRORCODE_None;
	}
//	printf("Bad token: %d\n", token);
	return ERRORCODE_IllegalType;
}

/*
// Build a style change. 'this' is the old style.
*/

ERRORCODE TextStyle::build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_style)
{
	TEXT_TOKEN token;

	if ((token = valid_token((TEXT_TOKEN)scp->token)) != TOKEN_Illegal)
	{
		helper[token]->build_change(scp, new_style, this);
		return ERRORCODE_None;
	}
//	printf("Bad token: %d\n", token);
	return ERRORCODE_IllegalType;
}

/*
// Say whether the style is undefined for this token.
// Returns TRUE, FALSE, or ERRORCODE_IllegalType
*/

SHORT TextStyle::test_style(TEXT_TOKEN token)
{
/* See if the token value is undefined. */

	if ((token = valid_token(token)) != TOKEN_Illegal)
	{
		return helper[token]->test_style(this);
	}
//	printf("Bad token: %d\n", token);
	return ERRORCODE_IllegalType;
}

/*
// Say whether the passed style matches us for this token.
// Returns TRUE, FALSE, or ERRORCODE_IllegalType
*/

SHORT TextStyle::compare_styles(TEXT_TOKEN token, TextStylePtr style)
{
/* See if the token value is undefined. */

	if ((token = valid_token(token)) != TOKEN_Illegal)
	{
		return helper[token]->compare_styles(this, style);
	}
//	printf("Bad token: %d\n", token);
	return ERRORCODE_IllegalType;
}

/*
// Make a style attribute undefined.
*/

ERRORCODE TextStyle::make_undefined(TEXT_TOKEN token)
{
/* Make undefined the token. */

	if ((token = valid_token(token)) != TOKEN_Illegal)
	{
		helper[token]->make_undefined(this);
		return ERRORCODE_None;
	}
//	printf("Bad token: %d\n", token);
	return ERRORCODE_IllegalType;
}

/*
// Set the face for a text style structure.
*/

VOID TextStyle::set_face(DB_RECORD_NUMBER face)
{
	database->free_face(record.face, temporary);
	database->inc_face(record.face = face, temporary);
}

/*
// Style font style access.
//
// We need to do some tricky stuff here to make separate bold and italic
// attributes. The trick is how to deal with the undefined state.
// The old scheme would set the font style field to -1 if bold *or* italic
// styles were different. Now we need to maintain the undefined-ness for each.
//
// We use the lower two bits as before for bold and italic. In addition, we
// now also use the two upper bits as "undefined" bits for bold and italic.
// We no longer use the whole value to store undefined for either (though we
// support this transparently for someone accessing the old "style" method).
*/

FONT_STYLE TextStyle::get_font_style(VOID) const
{
	return (record.font_style & (FONT_STYLE_Bold_Undefined | FONT_STYLE_Italic_Undefined))
				? FONT_STYLE_Undefined
				: (FONT_STYLE)(record.font_style & 3);
}

void TextStyle::set_font_style(FONT_STYLE font_style)
{
	record.font_style = font_style;
}

int TextStyle::GetBoldStyle(void) const
{
	return (record.font_style & FONT_STYLE_Bold_Undefined)
			? -1
			: (record.font_style & FONT_STYLE_Bold) != 0;
}

int TextStyle::GetItalicStyle(void) const
{
	return (record.font_style & FONT_STYLE_Italic_Undefined)
			? -1
			: (record.font_style & FONT_STYLE_Italic) != 0;
}

void TextStyle::SetBoldStyle(int nBold)
{
	if (nBold == -1)
	{
		record.font_style |= FONT_STYLE_Bold_Undefined;
	}
	else
	{
		record.font_style &= ~(FONT_STYLE_Bold_Undefined | FONT_STYLE_Bold);
		if (nBold)
		{
			record.font_style |= FONT_STYLE_Bold;
		}
	}
}

void TextStyle::SetItalicStyle(int nItalic)
{
	if (nItalic == -1)
	{
		record.font_style |= FONT_STYLE_Italic_Undefined;
	}
	else
	{
		record.font_style &= ~(FONT_STYLE_Italic_Undefined | FONT_STYLE_Italic);
		if (nItalic)
		{
			record.font_style |= FONT_STYLE_Italic;
		}
	}
}

/*
// Read the text style.
*/

ERRORCODE TextStyle::read(StorageDevicePtr device)
{
	ST_DEV_IO_SIZE size_result;
	ERRORCODE errorcode;

	errorcode = device->read_record(&record, sizeof (record), &size_result);

	if (errorcode == ERRORCODE_None)
	{
		if (size_result <= RecordSize_OldLineSpacing())
		{
			// Old document does not have new line spacing info, fill in as well
			// as we can.
			record.m_lOldPMGLineSpacing = record.font_ascend + record.font_descend;
			record.m_lWin3LineSpacing = record.m_lOldPMGLineSpacing;
			record.m_lLineSpacing = LINE_SPACING_OldPmg;
		}
	}
	return errorcode;
}

/*
// Write the text style.
*/

ERRORCODE TextStyle::write(StorageDevicePtr device)
{
	return device->write_record(&record, sizeof(record));
}

/*
// Return the size of the text style.
*/

ST_MAN_SIZE TextStyle::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(record));
}
