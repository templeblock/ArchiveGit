/*
// $Header: /PM8/App/PARAREC.H 1     3/03/99 6:08p Gbeddow $
//
// Paragraph record definitions.
//
// $Log: /PM8/App/PARAREC.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:45:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:00   FRED
// Initial revision.
// 
//    Rev 2.6   14 Sep 1995 09:32:20   JAY
// 32-bit changes
// 
//    Rev 2.5   01 May 1995 08:42:48   JAY
// Now handles macros correctly in assign().
// 
//    Rev 2.4   25 Apr 1995 09:57:26   JAY
// Made assign() public.
// 
//    Rev 2.3   19 Apr 1995 12:31:40   JAY
//  
// 
//    Rev 2.2   29 Mar 1995 08:58:58   JAY
// Lots of changes
// 
//    Rev 2.1   17 Feb 1995 16:45:16   JAY
// Added insert_character() overload which takes a style.
// 
//    Rev 2.0   07 Feb 1995 15:48:50   JAY
// Initial revision.
// 
//    Rev 1.7   30 Nov 1994 16:38:46   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.6   01 Jun 1994 16:35:20   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.5   23 May 1994 08:18:00   JAY
// Added a destroy() method which destroys the style when the paragraph record
// is permanently deleted from the database.
// 
//    Rev 1.4   18 Mar 1994 09:53:36   MACDUFF
// Publicized a set_new_data_defaults() method for conversion of
// old-old (DOS) documents.
// 
//    Rev 1.3   16 Mar 1994 16:02:22   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.2   06 Jan 1994 16:06:34   JAY
// Added maximum_characters variable.
// 
//    Rev 1.1   05 Jan 1994 08:08:22   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:52   JAY
//  
*/

#ifndef __PARAREC_H__
#define __PARAREC_H__

#include "textarry.h"

#define PARAGRAPH_FLAG_HasMacro	1

/*
// The paragraph record.
*/

FARCLASS ParagraphRecord : public DatabaseRecord
{
	friend class TextFlow;
protected:

/*
// Private data.
*/

	struct
	{
#ifdef WIN32
		USHORT fHasNewWordData;
#else
		unsigned int fHasNewWordData:1;
#endif
		WORD m_wFlags;
	} record;

	TextStyle style;		/* paragraph style for this paragraph. */
	Array text;				/* array of text data. (CHARACTER) */
	TextWordArray	word;				/* array of words. (TEXT_WORD) */
	Array change;			/* array of style changes. (STYLE_CHANGE) */

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	ParagraphRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Macro fixup helper during an assignment.
*/

	void FixupMacros(PMGDatabase* pSrcDatabase);

public:

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

/*
// The maximum paragraph size (in characters).
*/

	static SHORT maximum_characters;

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

/*
// Set the style for the paragraph.
*/

	VOID set_style(TextStyleRef s)
		{ style = s; }

/*
// Get the style for the paragraph.
*/

	TextStyleRef get_style(VOID)
		{ return style; }

/*
// Insert a character into the paragraph.
*/

	ERRORCODE insert_character(C_INDEX pos, CHARACTER c);

/*
// Insert a character (with style) into the paragraph.
*/

	ERRORCODE insert_character(C_INDEX pos, CHARACTER c, TextStyle* pStyle);

/*
// Delete a character from the paragraph.
//
// If pDeletedMacro != NULL, then it must point to an *initialized* BOOL
// variable (typically FALSE). This routine will not set it to FALSE, only
// TRUE as is appropriate.
*/

	ERRORCODE delete_character(C_INDEX pos, BOOL* pDeletedMacro = NULL);

/*
// Get a pointer to the desired character.
// NULL means that was not a good word to ask for.
*/

	CHARACTER_PTR get_character(C_INDEX c_index)
		{ return (CHARACTER_PTR)text.get_element(c_index); }

/*
// Return the number of characters in the paragraph.
*/

	SHORT number_of_characters(VOID)
		{ return text.count(); }

/*
// Empty all characters from the paragraph.
*/

	VOID empty_characters(VOID)
		{ text.empty_array(); }

/*
// Insert a word into the paragraph.
*/

	ERRORCODE insert_word(W_INDEX pos, TEXT_WORD_PTR w)
		{ return word.insert_element(w, pos); }

/*
// Delete a word from the paragraph.
*/

	ERRORCODE delete_word(W_INDEX pos)
		{ return word.delete_element(pos); }

/*
// Get a pointer to the desired word.
// NULL means that was not a good word to ask for.
*/

	TEXT_WORD_PTR get_word(W_INDEX w_index)
		{ return (TEXT_WORD_PTR)word.get_element(w_index); }

/*
// Return the number of words in the paragraph.
*/

	SHORT number_of_words(VOID)
		{ return word.count(); }

/*
// Empty all words from the paragraph.
*/

	VOID empty_words(VOID)
		{ word.empty_array(); }

/*
// Insert a style change into the paragraph.
*/

	ERRORCODE insert_change(S_INDEX pos, STYLE_CHANGE_PTR s);

/*
// Delete a style change from the paragraph.
*/

	ERRORCODE delete_change(S_INDEX pos);

/*
// Insert a style change checking for temporary styles, etc.
*/

	S_INDEX insert_style_change(S_INDEX s_index, STYLE_CHANGE_PTR scp);

/*
// Return a pointer to the desired style change.
*/

	STYLE_CHANGE_PTR get_style_change(S_INDEX which)
		{ return (STYLE_CHANGE_PTR)change.get_element(which); }

/*
// Return the number of style changes in the paragraph.
*/

	SHORT number_of_changes(VOID)
		{ return change.count(); }

/*
// Empty all words from the paragraph.
*/

	VOID empty_changes(VOID)
		{ change.empty_array(); }

/*
// Return the word containing a character.
*/

	W_INDEX word_of_character(C_INDEX c_index, TEXT_WORD_PTR wp_ret);

/*
// Convert a character range to a word range.
*/

	SHORT crange_to_wrange(CHARACTER_RANGE crange, WORD_RANGE_PTR wrange);

/*
// Get the image for a text word.
*/

	SHORT get_word_image(W_INDEX w_index, TEXT_WORD_PTR wp);

/*
// Size words in the paragraph.
*/

	ERRORCODE size_words(WORD_RANGE wrange, BOOL force, BOOL fSizeMacros = FALSE, BOOL* pSized = NULL);

/*
// Adjust the style tokens after a text change.
*/

	ERRORCODE adjust_style(CHARACTER_DELTA cdelta);

/*
// Change the style of a range to what is passed.
//
// Returns:
//			 0 = no tokens changed
//			>0 = at least 1 token changed
*/

	SHORT change_range_style(CHARACTER_RANGE crange, TextStylePtr new_style, BOOL temporary);

/*
// Remove all temporary tokens in a paragraph.
//
// Returns:
//			 0 = no tokens removed
//			>0 = at least 1 token removed
*/

	SHORT remove_temporary_tokens(VOID);

/*
// Make style changes in a paragraph permanent.
*/

	SHORT make_changes_permanent(VOID);

/*
// Macro support.
*/

	BOOL HasMacro(void);
	void UpdateMacroState(CHARACTER_RANGE* pRange = NULL);

	BOOL ResizeMacros(WORD_RANGE& wrange, CWordArray* pMacros = NULL);
	// Returns whether it sized anything.

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	ERRORCODE	ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the record.
*/

	ERRORCODE	WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the record.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr device);

/*****************************************
  Backward compatibility with old documents (before PMW 1.00.07)
******************************************/

	void set_new_data_defaults()
		{ word.set_new_data_defaults(); }

};

#endif					// #ifndef __PARAREC_H__
