/*
// $Header: /PM8/App/FRAMEREC.H 1     3/03/99 6:05p Gbeddow $
//
// Frame record definitions.
//
// $Log: /PM8/App/FRAMEREC.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 4     1/21/99 12:02p Cschendel
// Added a ETextWrapGutter type to the framerec for serialization and the
// the creation struct. Old files will have not text wrap gutter but newer
// files will have the default setting of the creation struct wich is
// GUTTER_medium
// 
// 3     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 2     4/29/98 6:00p Rlovejoy
// Draws use fill objects.
// 
//    Rev 1.0   14 Aug 1997 15:21:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:58   Fred
// Initial revision.
// 
//    Rev 1.27   28 May 1997 11:05:52   Fred
// Frame background effects
// 
//    Rev 1.26   23 May 1997 17:09:46   Fred
// New effects
// 
//    Rev 1.25   24 Mar 1997 16:14:34   Jay
// Got rid of a memory leak.
// 
//    Rev 1.24   17 Mar 1997 14:48:24   Jay
// Warp shape palette and support.
// 
//    Rev 1.23   12 Mar 1997 16:52:18   Fred
// Up/Down arrows now work in warped and rotated text boxes
// 
//    Rev 1.22   12 Mar 1997 14:54:42   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.21   09 Jan 1997 16:54:24   Fred
//  
// 
//    Rev 1.20   08 Jan 1997 15:22:22   Fred
// More text box changes
// 
//    Rev 1.19   06 Jan 1997 10:15:30   Fred
// Start of new text object
// 
//    Rev 1.18   18 Jul 1996 11:48:12   Jay
// ReleaseSubrecords()
// 
//    Rev 1.17   15 Jul 1996 17:46:00   Jay
//  
// 
//    Rev 1.16   12 Jul 1996 16:47:50   Jay
// Text wrap
// 
//    Rev 1.15   10 Jul 1996 10:23:50   Jay
// Text states no longer allocated.
// 
//    Rev 1.14   08 Jul 1996 17:51:32   Jay
// Linked frames, etc.
// 
//    Rev 1.13   03 Jul 1996 14:30:26   Jay
//  
// 
//    Rev 1.12   27 Jun 1996 13:27:38   Jay
//  
// 
//    Rev 1.11   26 Jun 1996 14:21:38   Jay
//  
// 
//    Rev 1.10   26 Jun 1996 09:06:26   Jay
//  
// 
//    Rev 1.9   25 Jun 1996 13:13:16   Jay
//  
// 
//    Rev 1.8   24 Jun 1996 18:06:14   Jay
//  
// 
//    Rev 1.7   21 Jun 1996 17:49:38   Jay
//  
// 
//    Rev 1.6   21 Jun 1996 14:30:26   Jay
// Stretch text, etc.
// 
//    Rev 1.5   20 Jun 1996 16:15:56   Jay
// Frame refresh offsets
// 
//    Rev 1.4   19 Jun 1996 17:32:12   Jay
//  
// 
//    Rev 1.3   13 Jun 1996 15:53:04   Jay
//  
// 
//    Rev 1.2   04 Jun 1996 17:38:56   Jay
//  
// 
//    Rev 1.1   24 May 1996 09:26:56   Jay
// New text code.
// 
//    Rev 1.0   14 Mar 1996 13:42:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:10   FRED
// Initial revision.
// 
//    Rev 2.2   14 Sep 1995 09:32:18   JAY
// 32-bit changes
// 
//    Rev 2.1   25 Apr 1995 08:51:28   JAY
// Added y-stretch capabilities.
// 
//    Rev 2.0   07 Feb 1995 15:48:36   JAY
// Initial revision.
// 
//    Rev 1.6   30 Nov 1994 16:35:12   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.5   01 Jun 1994 16:35:20   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.4   18 Mar 1994 09:53:18   MACDUFF
// Publicized a set_new_data_defaults() method for conversion of
// old-old (DOS) documents.
// 
//    Rev 1.3   17 Mar 1994 17:08:16   MACDUFF
// compute_flow_bounds() is a method now.
// 
//    Rev 1.2   16 Mar 1994 16:01:42   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.1   05 Jan 1994 08:08:20   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:50   JAY
//  
*/

class CFrameRecord;
class CWarpField;

class CLineArrayRecord;
class CWordArrayRecord;

#ifndef __FRAMEREC_H__
#define __FRAMEREC_H__

#include "text.h"
#include "textarry.h"
#include "arrayrec.h"
#include "outpath.h"

/*
// The frame data record.
*/

// WARNING: Old frame record. The new one is CFrameRecord.
class FrameRecord : public DatabaseRecord
{
	friend class TextFlow;

private:
	struct
	{
		FLAGS flags;
	   DB_RECORD_NUMBER paragraph;	/* inode of the paragraph for this frame. */
	   PBOX  bound;						/* bounding box of frame. */
		PPNT stretch_dims;				/* original frame dims when stretching turned on. */
	/*
 	// This box describes the area occupied by the frame for purposes of
	//	redisplay.  It may be larger or smaller than the frame "bounds".
	//	Its coordinates are relative to the frame origin.
 	*/
		PBOX			refresh_offsets;
		SHORT			has_new_line_data;			// BOOL
	} record;

	TextLineArray	line;							/* Array of lines (LINE) */

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	FrameRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

	void compute_flow_bounds(ParagraphPtr paragraph, PPNT frame_dims, PPNT_PTR text_extent_out, BOOL do_flow);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Insert a line into the frame.
*/

	ERRORCODE insert_line(L_INDEX pos, LINE_PTR l)
		{ return line.insert_element(l, pos); }

/*
// Delete a line from the frame.
*/

	ERRORCODE delete_line(L_INDEX pos)
		{ return line.delete_element(pos); }

/*
// Get a pointer to the desired word.
// NULL means that was not a good word to ask for.
*/

	LINE_PTR get_line(L_INDEX l_index)
		{ return (LINE_PTR)line.get_element(l_index); }

/*
// Return the number of lines in this frame.
*/

	SHORT number_of_lines(VOID)
		{ return line.count(); }

/*
// Empty all lines from the frame.
*/

	VOID empty_lines(VOID)
		{ line.empty_array(); }

/*
// Get the paragraph number for this frame.
*/

	DB_RECORD_NUMBER get_paragraph(VOID)
 		{ return record.paragraph; }

/*
// Set the paragraph number for this frame.
*/

	VOID set_paragraph(DB_RECORD_NUMBER p)
 		{ record.paragraph = p; }

/*
// Get the bound for this frame.
*/

	PBOX get_bound(VOID)
 		{ return record.bound; }

/*
// Set the bound for this frame.
*/

	VOID set_bound(PBOX bound)
 		{ record.bound = bound; }

/*
// Get the flags for this frame.
*/

	FLAGS get_flags(VOID)
 		{ return record.flags; }

/*
// Set the flags for this frame.
*/

	VOID set_flags(FLAGS flags)
 		{ record.flags = flags; }

/*
// Get the stretch dims for this frame.
*/

	PPNT get_stretch_dims(VOID)
 		{ return record.stretch_dims; }

/*
// Set the stretch dims for this frame.
*/

	VOID set_stretch_dims(PPNT dims)
 		{ record.stretch_dims = dims; }

/*
// Get the refresh offsets for this frame.
*/

	PBOX get_refresh_offsets(VOID)
 		{ return record.refresh_offsets; }

/*
// Set the refresh offsets for this frame.
*/

	VOID set_refresh_offsets(PBOX offsets)
 		{ record.refresh_offsets = offsets; }

/*
// Get the image of a line.
*/

	SHORT get_line_image(L_INDEX l_index, LINE_PTR lp);

/*
// Compute the line containing the passed word.
*/

	L_INDEX line_of_word(W_INDEX w_index, LINE_PTR lp_ret);

/*
// Stretch text support.
*/

	ERRORCODE stretch_text(VOID);

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
		{ line.set_new_data_defaults(); }

};

#define	FRAME_FLAG_stretch_text			1		/* Stretch the text */
#define	FRAME_FLAG_stretch_frame		2		/* Stretch the frame */
#define	FRAME_FLAG_use_white_space		4		/* Use white space */
#define	FRAME_FLAG_fit_text				8		/* Fit text into frame */
#define	FRAME_FLAG_no_initial_break	16		/* No initial break char */
#define	FRAME_FLAG_ystretch_frame		32		// Stretch the frame in the
															// y direction also. This
														// flag requires stretch_frame.
#define MINIMUM_SHRINK_SIZE	12


/////////////////////////////////////////////////////////////////////////////
// CFrameLocation

struct CFrameLocation
{
	DB_RECORD_NUMBER m_lFrame;			// What frame are we in?

	CHARACTER_INDEX m_lCharacter;

	LINE_INDEX m_lLine;
	CHARACTER_INDEX m_lFirstCharacterInLine;
	WORD_INDEX m_lFirstWordInLine;
	CTextLine m_Line;

	WORD_INDEX m_lWord;
	CHARACTER_INDEX m_lFirstCharacterInWord;
	CTextWord m_Word;

	PCOORD m_ColumnLeft;
	PCOORD m_ColumnRight;
};

/////////////////////////////////////////////////////////////////////////////
// CFrameRecord

/*
// The new frame data record.
*/

class CFrameRecord : public DatabaseRecord
{
public:
/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// The destructor for this record.
*/

	virtual ~CFrameRecord(void);

	// Frame Flags
	DWORD FrameFlags(void) const
		{ return m_Record.m_dwFrameFlags; }
	void FrameFlags(DWORD dwFlags)
		{ m_Record.m_dwFrameFlags = dwFlags; }

	// Line
	LINE_COUNT NumberOfLines(void);
	void InsertLine(LINE_INDEX Index, const CTextLine* pLine, LINE_COUNT Count = 1);
	void DeleteLine(LINE_INDEX Index, LINE_COUNT Count = 1);
	CTextLine* GetLine(LINE_INDEX Index);
	CTextLine* GetLine(LINE_INDEX Index, LINE_COUNT* pElementsAvailable);

	// Word
	WORD_COUNT NumberOfWords(void);
	void InsertWord(WORD_INDEX Index, const CTextWord* pWord, WORD_COUNT Count = 1);
	void DeleteWord(WORD_INDEX Index, WORD_COUNT Count = 1);
	CTextWord* GetWord(WORD_INDEX Index);
	CTextWord* GetWord(WORD_INDEX Index, WORD_COUNT* pWordsAvailable);

/*
// Text routines.
*/

	// Text record.
	void TextRecord(DB_RECORD_NUMBER lText)
		{ m_Record.m_lText = lText; }
	DB_RECORD_NUMBER TextRecord(void) const
		{ return m_Record.m_lText; }
	ERRORCODE CreateTextRecord(void);
	void DeleteTextRecord(void);

	CTextRecord* LockTextRecord(ERRORCODE* pError = NULL);

	// Text state.
	CTextState& TextState(void)
		{ return m_Record.m_TextState; }

	// Navigation.

	void GetCharacterLocation(CFrameLocation* pLocation);
	void GetPointLocation(PPNT p, CFrameLocation* pLocation, CFrameObject* pFrameObject, BOOL fNormalized = FALSE);
	CHARACTER_INDEX FirstCharacter(void)
		{ return TextState().m_lCharacter; }
	CHARACTER_COUNT NumberOfCharacters(void);

/*
// Object properties reflected down to here.
*/

	const PBOX& ObjectBounds(void) const
		{ return m_Record.m_ObjectBounds; }
	void ObjectBounds(const PBOX& Bounds)
		{ m_Record.m_ObjectBounds = Bounds; }

	WORD ObjectFlags(void) const
		{ return m_Record.m_wObjectFlags; }
	void ObjectFlags(WORD wFlags)
		{ m_Record.m_wObjectFlags = wFlags; }

	int VerticalAlignment(void) const
		{ return m_Record.m_nVerticalAlignment; }
	void VerticalAlignment(int nVerticalAlignment)
		{ m_Record.m_nVerticalAlignment = nVerticalAlignment; }

/*
// Linked frame navigation.
*/

	DB_RECORD_NUMBER PreviousFrame(void) const
		{ return m_Record.m_lPreviousFrame; }
	void PreviousFrame(DB_RECORD_NUMBER lPreviousFrame)
		{ m_Record.m_lPreviousFrame = lPreviousFrame; }
	DB_RECORD_NUMBER NextFrame(void) const
		{ return m_Record.m_lNextFrame; }
	void NextFrame(DB_RECORD_NUMBER lNextFrame)
		{ m_Record.m_lNextFrame = lNextFrame; }

	CFrameRecord* LockPreviousFrame(void);
	CFrameRecord* LockNextFrame(void);

	void AttachTextRecord(DB_RECORD_NUMBER lText);

/*
// The page we're on.
*/

	DB_RECORD_NUMBER Page(void) const
		{ return m_Record.m_lPage; }
	void Page(DB_RECORD_NUMBER lPage)
		{ m_Record.m_lPage = lPage; }

/*
// Get the refresh offsets for this frame.
*/

	PBOX RefreshOffsets(void) const
 		{ return m_Record.m_RefreshOffsets; }

/*
// Update the refresh offsets in the frame.
*/
	void UpdateRefreshOffsets(void);

	// Stretching management.

	PPNT StretchDims(void)
 		{ return m_Record.m_StretchDims; }
	void StretchDims(PPNT Dims)
 		{ m_Record.m_StretchDims = Dims; }

	// Margin support.
	CFixedRect& Margins(void)
		{ return m_Record.m_Margins; }
	void Margins(const CFixedRect& NewMargins)
		{ m_Record.m_Margins = NewMargins; }

	// Column support.
	int Columns(void) const
		{ return m_Record.m_nColumns; }
	void Columns(int nColumns)
		{ m_Record.m_nColumns = nColumns; }
	CFixed ColumnSpacing(void) const
		{ return m_Record.m_lColumnSpacing; }
	void ColumnSpacing(CFixed lColumnSpacing)
		{ m_Record.m_lColumnSpacing = lColumnSpacing; }

	// Get the column data for this frame.
	// ColumnWidth is the width of a column excluding any spacing (since the
	// spacing varies depending on the column!)

	void GetColumnData(int* pColumns, PCOORD* pColumnSpacing, PCOORD* pColumnWidth, PBOX* pBound = NULL);

	// General frame attributes support.
	void GetAttributes(CFrameAttributes* pAttributes);
	void SetAttributes(const CFrameAttributes& pAttributes);

	// Set our data given an old frame (like a conversion).
	ERRORCODE FromOldFrame(FrameRecord* pOldFrame);

	// Get the intersections path.
	COutlinePath& Intersections(void)
		{ return m_Intersections; }

	// Figure out the X and Y shadow offsets.
	virtual void GetShadowOffsets(PCOORD* pX, PCOORD* pY);

	// Figure out the X and Y outline widths.
	virtual void GetOutlineWidth(PCOORD* pX, PCOORD* pY);
	
	// Get a pointer to the current warp field object.
	CWarpField* GetWarpField(void);

	// Get references to the effects descriptions.
	FillFormatV1& Fill(void)
		{ return m_Record.m_Fill; }
	OutlineFormat& Outline(void)
		{ return m_Record.m_Outline; }
	ShadowFormat& Shadow(void)
		{ return m_Record.m_Shadow; }

	ETextWrapGutter GetGutter()
		{ return( (ETextWrapGutter)m_Record.m_eGutterType );	}

	void SetGutter( ETextWrapGutter eType )
		{	m_Record.m_eGutterType = eType; }

protected:
	struct
	{
		DB_RECORD_NUMBER m_lText;				// What is our text record?
		DB_RECORD_NUMBER m_lPreviousFrame;	// What frame is linked to us?
		DB_RECORD_NUMBER m_lNextFrame;		// What is the next linked frame?
		DB_RECORD_NUMBER m_lPage;				// What page are we on?

		DB_RECORD_NUMBER m_lLineArray;		// Our array of lines
		DB_RECORD_NUMBER m_lWordArray;		// Our array of words

		// The text state.
		CTextState m_TextState;					// Text state id.

		PBOX m_ObjectBounds;						// The bounds of our parent object
		WORD m_wObjectFlags;						// The flags of our parent object
		DWORD m_dwFrameFlags;					// The frame-specific flags
		PBOX m_RefreshOffsets;

		short int m_nVerticalAlignment;		// Vertical alignment.

		// Stretching support.
		PPNT m_StretchDims;

		// Margin support.
		CFixedRect m_Margins;

		// Column support.
		short int m_nColumns;
		CFixed m_lColumnSpacing;

	// Added with PMG 4.0
		DB_RECORD_NUMBER m_WarpField;			// inode of warp field

		// New Frame Effects (Added 5/16/97 for PMG 4.0)
		SHORT m_nEffectsVersion;					// 0 = PMG 3.0
															// 1 = PMG 4.0
															// 2 = PMG 5.0
		FillFormatV1 m_Fill;
		OutlineFormat m_Outline;
		ShadowFormat m_Shadow;

		short int m_eGutterType;			// added PM8, amount of gutter between text and avoided object

	} m_Record;

	COutlinePath m_Intersections;

	CLineArrayRecord* m_pLineArray;
	CWordArrayRecord* m_pWordArray;

	void LockLineArray(void);
	void LockWordArray(void);

	// Pointer to warp field object. Allocated by GetWarpField().
	CWarpField* m_pWarpField;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CFrameRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

public:
	// Destroy any records we may have allocated.
	void DestroyRecords(void);

	// The database is going away. All records are being freed.
	// Release any sub-records we have locked.
	virtual void ReleaseSubrecords(void);

	// Free the warp field.
	void FreeWarpField(void);

/********************************/
/* Implementation Overridables. */
/********************************/

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

/////////////////////////////////////////////////////////////////////////////
// Warp field support.

// Set a new warp field, freeing an old one.

	VOID set_new_warp_field(DB_RECORD_NUMBER l);
	
	DB_RECORD_NUMBER GetWarpFieldRecord(void) const
		{ return m_Record.m_WarpField; }

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
};

/////////////////////////////////////////////////////////////////////////////
// CLineArrayRecord

class CLineArrayRecord : public CArrayRecord
{
private:
/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CLineArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);
};

/////////////////////////////////////////////////////////////////////////////
// CWordArrayRecord

class CWordArrayRecord : public CArrayRecord
{
private:
/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CWordArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);
};

#endif					// #ifndef __FRAMEREC_H__
