/*
// $Header: /PM8/App/CTXP.H 1     3/03/99 6:04p Gbeddow $	
//
// Text pointer (insertion point) definitions.
//
// $Log: /PM8/App/CTXP.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     3/01/99 7:13p Psasse
// Support for Vertical Banner Text
// 
// 2     12/04/97 2:25p Hforman
// CArray -> CVarArray
// 
//    Rev 1.0   14 Aug 1997 15:19:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:20   Fred
// Initial revision.
// 
//    Rev 1.13   16 Jul 1997 13:19:46   Fred
// Fixed text selection bugs
// 
//    Rev 1.12   06 Aug 1996 10:43:04   Jay
// Changes to CTxp
// 
//    Rev 1.11   31 Jul 1996 18:21:08   Jay
// Fixes, etc!
// 
//    Rev 1.10   22 Jul 1996 18:06:32   Jay
// SelectText can now not set DesiredX
// 
//    Rev 1.9   22 Jul 1996 13:06:08   Jay
// New text select mechanism
// 
//    Rev 1.8   09 Jul 1996 15:37:12   Jay
//  
// 
//    Rev 1.7   25 Jun 1996 13:13:14   Jay
//  
// 
//    Rev 1.6   25 Jun 1996 10:25:30   Jay
//  
// 
//    Rev 1.5   24 Jun 1996 18:06:04   Jay
//  
// 
//    Rev 1.4   21 Jun 1996 14:30:20   Jay
// Stretch text, etc.
// 
//    Rev 1.3   20 Jun 1996 14:59:44   Jay
//  
// 
//    Rev 1.2   17 Jun 1996 12:01:24   Jay
//  
// 
//    Rev 1.1   13 Jun 1996 15:53:00   Jay
//  
// 
//    Rev 1.0   04 Jun 1996 17:38:54   Jay
//  
*/

class CTxp;

#ifndef __CTXP_H__
#define __CTXP_H__

#include "carray.h"
#include "text.h"

struct CFrameLocation;
class CFrameObject;
class CFrameRecord;
class CTextRecord;

/*
// A text pointer.
*/

class CTxp
{
public:
/*
// Functions for toggling parts of a txp.
// These allow toggling of the single cursor and toggling of the text select.
// Bits in the "which" TOGGLE variable may be OR'd for multiple action!
*/

	typedef void (*TOGGLE_FUNCTION)(CTxp* pTxp, int nWhich, LPVOID pToggleData);

	// Toggle flags
	enum
	{
		TOGGLE_Cursor = 1,
		TOGGLE_Select = 2,
		TOGGLE_All = TOGGLE_Cursor | TOGGLE_Select
	};

	// Flags
	enum
	{
		FLAG_start =			1,					/* At start of text */
		FLAG_end =				2,					/* At end of text */
		FLAG_stretch_text =	4,					/* Frame has stretched text */
		FLAG_stretch_frame =	8,					/* Frame stretchs */
		FLAG_can_stretch_text =	16,				/* Frame can stretch text */
	};

	// Selection ends
	enum
	{
		END_NoSelection,
		END_Left,
		END_Right,
		END_Unknown,
		END_Current							/* Not universally supported. */
	};

protected:
	PMGDatabase*			m_pDatabase;

	CFrameObject*			m_pObject;		// For external reference.

	PCOORD					m_DesiredX;
	DB_RECORD_NUMBER		m_lFrameRecord;
	DB_RECORD_NUMBER		m_lTextRecord;
	TOGGLE_FUNCTION		m_pToggleFunction;
	LPVOID					m_pToggleData;

	CHARACTER_INDEX		m_lIndex;
	CTextStyle				m_Style;
	CTextStyle				m_SelectStyle;		/* composite style for selected region */
	CFrameAttributes		m_FrameAttributes;

	int						m_nVisible;

	PBOX						m_CursorBox;
	int						m_nCursorColumn;
	DB_RECORD_NUMBER		m_lCursorFrame;

	CVarArray					m_SelectBounds;
	CVarArray					m_SelectFrames;
	BOOL						m_fSelectBoundsValid;
	BOOL						m_fIgnoreFrameBoundaries;

	CHARACTER_INDEX		m_lSelectStart;
	CHARACTER_INDEX		m_lSelectEnd;
	DB_RECORD_NUMBER		m_lSelectStartFrame;
	DB_RECORD_NUMBER		m_lSelectEndFrame;

	FLAGS						m_nFlags;
	BOOL						m_fTransition;

	BOOL						m_fWantFlowText;

/*
// Change lines.
*/

	ERRORCODE ChangeLine(LINE_COUNT lLineDelta, BOOL fShifted);

	void DeleteTextFromFrames(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd);

	void FindCharacterLocation(CFrameLocation* pLocation, int nStartBias, int nEndBias);

	void CommonInit(CFrameObject* pObject,
						 DB_RECORD_NUMBER lFrame,
						 PMGDatabase* pDatabase,
						 CHARACTER_INDEX lIndex,
						 TOGGLE_FUNCTION pToggleFunction,
						 LPVOID pToggleData);

public:

/*
// Constructor.
*/

	CTxp(PMGDatabasePtr ref);

/*
// Destructor.
*/

	virtual ~CTxp() {};

/*
// Return whether text is selected or not.
*/

	BOOL SomethingSelected(void)
		{ return m_lSelectStart != m_lSelectEnd; }

/*
// Get the database for this txp.
*/

	PMGDatabase* Database(void) const
 		{ return m_pDatabase; }

/*
// Get the database for this txp.
*/

	CFrameObject* Object(void) const
 		{ return m_pObject; }

/*
// Get the frame this txp is in.
*/

	DB_RECORD_NUMBER FrameRecord(void) const
		{ return m_lFrameRecord; }

/*
// Get the text record this txp is in.
*/

	DB_RECORD_NUMBER TextRecord(void)
		{ return m_lTextRecord; }

/*
// Get the txp's flags.
*/

	FLAGS Flags(void) const
		{ return m_nFlags; }

/*
// Get the txp's visible flags.
*/

	int Visible(void) const
		{ return m_nVisible; }

/*
// Get the cursor box.
*/

	PBOX CursorBox(void) const
		{ return m_CursorBox; }

/*
// Get the cursor column.
*/

	int CursorColumn(void) const
		{ return m_nCursorColumn; }

/*
// Get the cursor frame.
*/

	DB_RECORD_NUMBER CursorFrame(void) const
		{ return m_lCursorFrame; }

/*
// Get the transition flag.
*/

	BOOL Transition(void) const
		{ return m_fTransition; }

/*
// Get the ignore boundaries flag.
*/
	BOOL GetIgnoreBoundaries(void) const
		{return m_fIgnoreFrameBoundaries; }

/*
// Set the ignore boundaries flag.
*/
	void SetIgnoreBoundaries(const BOOL fSet)
		{m_fIgnoreFrameBoundaries = fSet; }
		
/*
// Set the transition flag.
*/

	void SetTransition(BOOL fTransition = TRUE)
		{ m_fTransition = fTransition; }

/*
// Set the select bound.
*/

	void UpdateSelectBounds(void);

	void InvalidateSelectBounds(void)
		{ m_fSelectBoundsValid = FALSE; }

	int NumberOfSelectBounds(void) const
		{ return m_SelectBounds.GetSize(); }
	const PBOX& SelectBound(int nBound) const
		{ return *(const PBOX*)m_SelectBounds.GetAt(nBound); }
	CFrameObject* SelectFrame(int nBound) const
		{ return *(CFrameObject**)m_SelectFrames.GetAt(nBound); }

	void ComputeSelectBounds(void);

/*
// Get the cursor index.
*/

	CHARACTER_INDEX Index(void) const
		{ return m_lIndex; }

/*
// Set the cursor index.
*/

	void Index(CHARACTER_INDEX lIndex)
		{ m_lIndex = lIndex; }

/*
// Get the start of the select.
*/

	CHARACTER_INDEX SelectStart(void) const
		{ return m_lSelectStart; }

/*
// Get the end of the select.
*/

	CHARACTER_INDEX SelectEnd(void) const
		{ return m_lSelectEnd; }

/*
// Get the style for this txp.
*/

	CTextStyle& Style(void)
		{ return m_Style; }

/*
// Set the style for this txp.
*/

	void Style(const CTextStyle& NewStyle)
		{ m_Style = NewStyle; }

/*
// Get the selection style for this txp.
*/

	CTextStyle& SelectStyle(void)
		{ return m_SelectStyle; }

/*
// Get the frame attributes for this txp.
*/
	CFrameAttributes& FrameAttributes(void)
		{ return m_FrameAttributes; }

/*
// Reset the txp.
*/

	void Reset(BOOL fSaveFace = FALSE);

/*
// Return whether the txp is valid or not.
*/

	BOOL Valid(void);

/*
// Attach a txp object to a frame.
*/

	void Init(CFrameObject* pObject, CHARACTER_INDEX offset, CTxp::TOGGLE_FUNCTION pToggleFunction = NULL, LPVOID pToggleData = NULL);
	void Init(DB_RECORD_NUMBER lFrame, PMGDatabase* pDatabase, CHARACTER_INDEX lIndex, TOGGLE_FUNCTION pToggleFunction = NULL, LPVOID pToggleData = NULL);

/*
// Move to a new frame while editing.
// This is used for navigating linked frames.
*/

	void NewFrame(CFrameObject* pObject);

/*
// Toggle the txp.
*/

	void Toggle(int nWhich = CTxp::TOGGLE_All);

/*
// Turn the all or part of a txp on.
*/

	void On(int nWhich = CTxp::TOGGLE_All);

/*
// Turn the all or part of a txp off.
*/

	void Off(int nWhich = CTxp::TOGGLE_All);

/*
// Set the position for a txp.
*/

	void SetPosition(CHARACTER_INDEX lNew, int nWhichEnd, BOOL fSetDesiredX);

/*
// Deselect all text selected by a txp.
*/

	void Deselect(void);

/*
// Select all text.
*/

	void SelectAll(void);

/*
// Select a range of text.
*/

	void SelectText(CHARACTER_INDEX lSelectStart, CHARACTER_INDEX lSelectEnd, BOOL fSetDesiredX = TRUE);

/*
// Get a location corresponding to the current position.
// Throws exceptions when necessary.
*/

	void GetFrameLocation(CFrameLocation* pLocation, DB_RECORD_NUMBER lFrame = -1);

/*
// Return the x offset corresponding to where the txp currently is.
*/

	PCOORD XOffset(CFrameLocation* pLocation = NULL);

/*
// Calculate the frame at the txp's current location.
*/

	ERRORCODE CalcFrame(BOOL fEnd = FALSE);

/*
// Calculate a cursor box for the txp.
*/

	ERRORCODE CalcCursor(void);

/*
// Compute the style at the txp's current location.
*/

	ERRORCODE CalcStyle(void);

/*
// Compute the frame attributes.
*/

	void CalcFrameAttributes(void);

/*
// Calculate a cursor box for the txp and the style at that point.
*/

	ERRORCODE CalcAll(void);

/*
// Set the desired x for a txp.
// This is the x position that is maintained when the cursor is moved up or
// down. The x position used is the txp's current x position.
*/

	void SetDesiredX(void);

/*
// Move the txp to a point.
// This is useful for the mouse interface.
*/

	ERRORCODE MoveToPoint(PPNT p, int nWhichEnd, BOOL fSetDesiredX);

/*
// Return the last offset available in the text.
*/

	CHARACTER_INDEX LastOffsetInText(void);

	// Pass -1 (or default) to use current Txp frame.
	CFrameRecord* LockFrame(DB_RECORD_NUMBER lFrame = -1);
	CTextRecord* LockText(void);

	// Reset the selection frames.
	void ResetSelectionFrames(void)
		{ m_lSelectStartFrame = m_lSelectEndFrame = m_lFrameRecord; }

	// Set the frame attributes.
	void SetFrameAttributes(const CFrameAttributes& Attributes);


	// Flow text in a particular range.
	void FlowText(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd);

	// Say whether we want FlowText() calls above to actually flow text or not.
	void WantFlowText(BOOL fWantFlowText)
		{ m_fWantFlowText = fWantFlowText; }

/*************************/
/* Character management. */
/*************************/

/*
// Insert a string into the text at the current txp location.
*/

	ERRORCODE InsertText(CHARACTER* s, CHARACTER_COUNT lCount, BOOL fSingle = FALSE);

	// 8-bit character version. Useful for jamming strings.
	ERRORCODE InsertString(LPCSTR pszText);

/*
// Delete characters in front of the current txp location.
*/

	ERRORCODE DeleteText(CHARACTER_COUNT lCount);

/*
// Delete characters in back of the current txp location.
*/

	ERRORCODE Backspace(CHARACTER_COUNT lCount);

/************************/
/* Paragraph management */
/************************/

	// Set the horizontal alignment for the selected paragraphs.
	BOOL SetHorizontalAlignment(ALIGN_TYPE Type);

/********************/
/* Frame management */
/********************/

	// Set the vertical alignment for the selected frames.
	BOOL SetVerticalAlignment(VERT_ALIGN_TYPE Type);

/****************************/
/* Cursor movement methods. */
/****************************/

/*
// Move the insertion point left.
*/

	void Left(BOOL fShifted);

/*
// Move the insertion point right.
*/

	void Right(BOOL fShifted);

/*
// Move the insertion point left a word.
*/

	void LeftWord(BOOL fShifted);

/*
// Move the insertion point right a word.
*/

	void RightWord(BOOL fShifted);

/*
// Move the insertion point up a line.
*/

	void Up(BOOL fShifted);

/*
// Move the insertion point down a line.
*/

	void Down(BOOL fShifted);

/*
// Move the insertion point to the beginning of current line.
*/

	void Bol(BOOL fShifted);

/*
// Move the insertion point to the end of current line.
*/

	void Eol(BOOL fShifted);

/*
// Move the insertion point to the beginning of text.
*/

	void Bot(BOOL fShifted);

/*
// Move the insertion point to the end of text.
*/

	void Eot(BOOL fShifted);

/*
// Return the first character index for the current line.
*/

	CHARACTER_INDEX FirstCharacterInLine(void);

/*
// Return the last character index for the current line.
*/

	CHARACTER_INDEX LastCharacterInLine(void);

/*
// Get the character range corresponding to this txp.
*/

	void GetRange(CHARACTER_INDEX* pStart, CHARACTER_INDEX* pEnd);

/*
// Get the paragraph range corresponding to this txp.
*/

	void GetParagraphRange(PARAGRAPH_INDEX* pStart, PARAGRAPH_INDEX* pEnd);

/*
// Get the frame range corresponding to this txp.
*/

	void GetFrameRange(DB_RECORD_NUMBER* pStart, DB_RECORD_NUMBER* pEnd);

/*
// Update the range within the domain of this txp after a style change.
*/

	void UpdateRange(CHARACTER_INDEX* pStart = NULL, CHARACTER_INDEX* pEnd = NULL);
};

#endif								/* #ifndef __CTXP_H__ */
