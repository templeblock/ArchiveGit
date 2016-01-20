/*
// $Header: /PM8/App/TEXTREC.H 1     3/03/99 6:12p Gbeddow $
//
// Text record definitions.
//
// $Log: /PM8/App/TEXTREC.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     3/01/99 11:55a Cschende
// added functions to make permanent font substitutions in the document
// after it is read in for font which are no longer going to be
// distributed with PrintMaster
// 
//    Rev 1.0   14 Aug 1997 15:26:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:14   Fred
// Initial revision.
// 
//    Rev 1.26   23 Jul 1997 11:14:04   Jay
// Fixed multiple old color tokens in paragraph.
// 
//    Rev 1.25   08 Jul 1997 14:47:04   Jay
// Fixed moving array element heap corruption
// 
//    Rev 1.24   07 Jul 1997 10:44:56   Jay
// Fixes old style changes
// 
//    Rev 1.23   04 Mar 1997 17:04:14   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.22   06 Jan 1997 10:13:40   Jay
// Optimizations
// 
//    Rev 1.21   10 Jul 1996 10:26:12   Jay
// Text states no longer allocated.
// 
//    Rev 1.20   09 Jul 1996 15:37:20   Jay
//  
// 
//    Rev 1.19   05 Jul 1996 10:47:16   Jay
//  
// 
//    Rev 1.18   02 Jul 1996 08:55:10   Jay
//  
// 
//    Rev 1.17   28 Jun 1996 16:18:38   Jay
//  
// 
//    Rev 1.16   27 Jun 1996 17:20:50   Jay
//  
// 
//    Rev 1.15   27 Jun 1996 13:28:18   Jay
//  
// 
//    Rev 1.14   26 Jun 1996 14:22:28   Jay
//  
// 
//    Rev 1.13   26 Jun 1996 09:06:58   Jay
//  
// 
//    Rev 1.12   24 Jun 1996 18:06:18   Jay
//  
// 
//    Rev 1.11   21 Jun 1996 14:30:34   Jay
// Stretch text, etc.
// 
//    Rev 1.10   20 Jun 1996 17:40:02   Jay
// Some speedups and diagnostics.
// 
//    Rev 1.9   19 Jun 1996 17:32:16   Jay
//  
// 
//    Rev 1.8   19 Jun 1996 13:45:52   Jay
//  
// 
//    Rev 1.7   18 Jun 1996 16:14:54   Jay
//  
// 
//    Rev 1.6   14 Jun 1996 17:05:44   Jay
//  
// 
//    Rev 1.5   13 Jun 1996 17:46:12   Jay
//  
// 
//    Rev 1.4   13 Jun 1996 15:53:12   Jay
//  
// 
//    Rev 1.3   04 Jun 1996 17:39:00   Jay
//  
// 
//    Rev 1.2   24 May 1996 09:27:04   Jay
// New text code.
// 
//    Rev 1.1   23 Apr 1996 08:07:42   Jay
// More new stuff
// 
//    Rev 1.0   12 Apr 1996 13:08:58   Jay
// New text stuff
*/

//
// Classes declared in this file.
//

class CTextRecord;

class CCharacterArrayRecord;
class CStyleArrayRecord;
class CParagraphArrayRecord;

#ifndef __TEXTREC_H__
#define __TEXTREC_H__

#include "arrayrec.h"

class ParagraphRecord;
class CFrameObject;

class PMGFontServer;
/////////////////////////////////////////////////////////////////////////////
// CTextRecord
//
// This is the mother text record.
// It contains just about anything you would want to know about text (except
// for those things which exist in the frame record, like lines).
//

class CTextRecord : public DatabaseRecord
{
protected:

/*
// Protected data.
*/

	enum
	{
		TOKEN_Revision30 = 0,		// 3.0 token revision.
		TOKEN_Revision = 1			// Current token revision.
	};

	struct
	{
		WORD m_wFlags;

		DB_RECORD_NUMBER	m_lCharacterArray;	// Character array record number
		DB_RECORD_NUMBER	m_lStyleArray;			// Style change array record number
		DB_RECORD_NUMBER	m_lParagraphArray;	// Paragraph array record number

		DB_RECORD_NUMBER	m_lFirstFrame;			// First frame we flow into.
		CFixed				m_lStretchNumerator;
		CFixed				m_lStretchDenominator;
		CFixed				m_lStretchExpansion;
		CHARACTER_COUNT	m_lMacroCount;			// How many fill-in fields?
		WORD					m_wTokenRevision;		// What token revision?
	} m_Record;

	// Another text record is overriding us while we are being edited.
	CTextRecord* m_pEditingText;

	CCharacterArrayRecord* m_pCharacterArray;
	CStyleArrayRecord* m_pStyleArray;
	CParagraphArrayRecord* m_pParagraphArray;

	void LockCharacterArray(void);
	void LockStyleArray(void);
	void LockParagraphArray(void);

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CTextRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Release any records we may have locked.
*/

	void ReleaseRecords(void);

	// Destroy any records we may have allocated.
	void DestroyRecords(void);

	// Text deletion helpers.
	void DeleteStyles(PARAGRAPH_INDEX lParagraph, CHARACTER_INDEX lStart, CHARACTER_COUNT lCount);

public:

	long m_lGetParagraph;
	long m_lModifyParagraph;
	long m_lGetParagraphStyle;
	long m_lGetStyleChange;
	long m_lModifyStyleChange;
	long m_lAdvanceTextState;

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// The destructor for this record.
*/

	virtual ~CTextRecord(void);

/*
// Show myself.
*/

	void Dump(void);

/*
// Test myself.
*/

	void SelfTest(void);
	void DumpCounts(void);

	// Convert the contents of an old text paragraph into this object.
	ERRORCODE FromOldParagraph(ParagraphRecord* pParagraph);

/***************/
/* Data Access */
/***************/

	// TextState
	void InitializeTextState(CTextState* pState, CHARACTER_INDEX Index, BOOL fForceReset = TRUE);
	void AdvanceTextState(CTextState* pState, CHARACTER_INDEX Index);

	// reflows all text in the record
	void Reflow();

	// Character
	CHARACTER_COUNT NumberOfCharacters(void);
	void InsertCharacter(CHARACTER_INDEX Index, const CHARACTER* c, CHARACTER_COUNT Count = 1);
	void DeleteCharacter(CHARACTER_INDEX Index, CHARACTER_COUNT Count = 1);
	CHARACTER* GetCharacter(CHARACTER_INDEX Index);
	CHARACTER* GetCharacters(CHARACTER_INDEX Index, CHARACTER_COUNT* pCharactersAvailable);

	// StyleChange
	STYLE_COUNT NumberOfStyleChanges(void);
	CTextStyleChange* NewStyleChange(STYLE_INDEX* plChange, const CTextStyleChange* pSourceChange = NULL);
	CTextStyleChange* GetStyleChange(STYLE_INDEX lChange);
	CTextStyleChange* ModifyStyleChange(STYLE_INDEX lChange);
	void DeleteStyleChange(STYLE_INDEX lChange, BOOL fAdjustCounts = TRUE);

	// Special variant that allows lChange == -1.
	CTextStyleChange* GetStyleChange(PARAGRAPH_INDEX lParagraph, STYLE_INDEX lChange);
	CTextStyleChange* ModifyStyleChange(PARAGRAPH_INDEX lParagraph, STYLE_INDEX lChange);

	// Paragraph
	PARAGRAPH_COUNT NumberOfParagraphs(void);
	void InsertParagraph(PARAGRAPH_INDEX Index, const CTextParagraph* pParagraph, PARAGRAPH_COUNT Count = 1);
	void DeleteParagraph(PARAGRAPH_INDEX Index, PARAGRAPH_COUNT Count = 1);
	CTextParagraph* GetParagraph(PARAGRAPH_INDEX Index);
	CTextParagraph* GetParagraph(PARAGRAPH_INDEX Index, PARAGRAPH_COUNT* pParagraphsAvailable);
	CTextParagraph* ModifyParagraph(PARAGRAPH_INDEX Index);
	void GetParagraphStyle(PARAGRAPH_INDEX Index, CTextStyle& Style, BOOL fApplyParagraphChanges = TRUE);
	void SetParagraphStyle(PARAGRAPH_INDEX lParagraph, const CTextStyle* pNewStyle);
	// Paragraph Helpers
	void ApplyParagraphChanges(CTextParagraph* pParagraph, CTextStyle& Style);
	void SplitParagraph(PARAGRAPH_INDEX lParagraph, CHARACTER_INDEX lSplit);
	void MergeParagraphs(PARAGRAPH_INDEX lParagraph);
	void DeleteParagraphStyles(CTextParagraph* pParagraph, STYLE_INDEX lLastChange = 0);

	// First frame access.
	DB_RECORD_NUMBER FirstFrame(void) const
		{ return m_Record.m_lFirstFrame; }
	void FirstFrame(DB_RECORD_NUMBER lFirstFrame)
		{ m_Record.m_lFirstFrame = lFirstFrame; }

	// Find the frame corresponding to a character location.
	// The frame is the current or recommended (best guess) frame.
	DB_RECORD_NUMBER GetFrameOfCharacter(CHARACTER_INDEX lCharacter, DB_RECORD_NUMBER lFrame = 0, BOOL fEnd = FALSE);

	// Stretching support.
	void SetStretch(CFixed lStretchNumerator, CFixed lStretchDenominator, CFixed lStretchExpansion);
	void ClearStretch(void);
	void ApplyStretch(CTextStyle& pStyle);

	CFixed StretchNumerator(void) const
		{ return m_Record.m_lStretchNumerator; }
	CFixed StretchDenominator(void) const
		{ return m_Record.m_lStretchDenominator; }
	CFixed StretchExpansion(void) const
		{ return m_Record.m_lStretchExpansion; }

/**********************************/
/* Higher-level Text Manipulation */
/**********************************/

	ERRORCODE InsertText(CHARACTER_INDEX lIndex, const CHARACTER* c, CHARACTER_COUNT lCount = 1, DB_RECORD_NUMBER lFrame = 0);
	ERRORCODE DeleteText(CHARACTER_INDEX lIndex, CHARACTER_COUNT lCount = 1, DB_RECORD_NUMBER lFrame = 0);

	void AdjustFrameStates(const CTextState* pChangeState,
								  CHARACTER_COUNT lCharacterDelta,
								  PARAGRAPH_COUNT lParagraphDelta,
								  DB_RECORD_NUMBER lFrame = 0);
	void AdjustTextState(CTextState* pState,
								const CTextState* pChangeState,
								CHARACTER_COUNT lCharacterDelta,
								PARAGRAPH_COUNT lParagraphDelta,
								const CTextState* pLastState = NULL);

/**********************************/
/* Higher-level Style Manipulation */
/**********************************/

/*
// Change the style of a range to what is passed.
//
// Returns:
//			 0 = no tokens changed
//			>0 = at least 1 token changed
*/
	int ChangeRangeStyle(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd, const CTextStyle* pNewStyle);
	// Helper function.
	int ChangeParagraphRangeStyle(const CTextState& pTextState, CHARACTER_INDEX lEnd, const CTextStyle* pNewStyle);

	// preforms a font substitution if neccessary on the style changes in the text record
	bool SubstituteFonts( PMGFontServer *pFontServer );

/*********************************************/
/* Higher-level Paragraph Style Manipulation */
/*********************************************/

	// Change the style of a range of paragraphs to the defined parts of the
	// style passed.
	int ChangeParagraphStyle(PARAGRAPH_INDEX lStart, PARAGRAPH_INDEX lEnd, const CTextStyle* pNewStyle);
	// Change the style of a range of paragraphs with the style change passed.
	int ChangeParagraphStyle(PARAGRAPH_INDEX lStart, PARAGRAPH_INDEX lEnd, const CTextStyle* pNewStyle, int Token);

	// Helper functions for the above.
	int ChangeParagraphStyle(CTextParagraph* pParagraph, PARAGRAPH_INDEX lParagraph, const CTextStyle* pBaseStyle, const CTextStyle* pNewStyle, int Token);

	// Change the tabs of a range of paragraphs.
	int ChangeParagraphTabs(PARAGRAPH_INDEX lStart, PARAGRAPH_INDEX lEnd, CTabArray* pTabs, BOOL fRelative = FALSE);
	// Change the tabs of a particular paragraph.
	int ChangeParagraphTabs(PARAGRAPH_INDEX lParagraph, CTextStyle* pBaseStyle, CTabArray* pTabs, BOOL fRelative);

	// Tab helper functions.
	void DeleteTabChanges(CTextParagraph* pParagraph, PARAGRAPH_INDEX lParagraph);
	void AddTab(CTextParagraph* pParagraph, CTabArray* pTabs, const CTextTab* pTab, PARAGRAPH_INDEX lParagraph);
	void RemoveTab(CTextParagraph* pParagraph, CTabArray* pTabs, const CTextTab* pTab, PARAGRAPH_INDEX lParagraph);
	void AddTabChange(CTextParagraph* pParagraph, const CTextTab* pTab, PARAGRAPH_INDEX lParagraph);

/**********************************/
/* Macro (fill-in field) routines */
/**********************************/

	// How many macro characters are in our text?
	CHARACTER_COUNT MacroCount(void) const
		{ return m_Record.m_lMacroCount; }

	// Do we have any macros?
	BOOL HasMacro(void) const
		{ return MacroCount() > 0; }

	// Reflow all macros in this frame.
	ERRORCODE ReflowMacros(CWordArray* pMacros = NULL, CFrameObject* pObject = NULL);

	// Get all macros in use.
	void GetMacrosInUse(CWordArray& cwaMacros);

	// Helper functions used during text record assignment.
	void FixupMacros(PMGDatabase* pSrcDatabase, CHARACTER_INDEX lStart = 0, CHARACTER_INDEX lEnd = -1);
	void ReferenceStyleChanges(STYLE_INDEX lChange, PMGDatabase* pSourceDatabase, PMGDatabase* pDestDatabase);
	BOOL ReferenceStyleChange(CTextStyleChange* pChange, PMGDatabase* pSourceDatabase, PMGDatabase* pDestDatabase);
	void CopyBaseStyle(CTextParagraph* pDestParagraph,
							 CTextParagraph* pSourceParagraph,
							 PMGDatabase* pSourceDatabase);
	void CopyParagraphFormatting(CTextParagraph* pDestParagraph,
										  CTextParagraph* pSourceParagraph,
										  CTextRecord* pSourceText,
										  BOOL fCopyCharacterFormatting = FALSE);
	void CopyCharacterFormatting(CTextParagraph* pDestParagraph,
										  CTextParagraph* pSourceParagraph,
										  CTextRecord* pSourceText);

	// Set the text record that is overriding us.
	void SetEditingText(CTextRecord* pText);

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

/*
// The database is going away. All records are being freed.
// Release any sub-records we have locked.
*/

	virtual void ReleaseSubrecords(void);

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

protected:
   void FixOldTokens(void);
   void FixOldTokens(STYLE_INDEX lChange, CTextStyle& Style, BOOL fParagraph);
   CTextStyleChange* AppendNewChange(STYLE_INDEX lChange, STYLE_INDEX* plNewChange);

};

/////////////////////////////////////////////////////////////////////////////
// CCharacterArrayRecord

class CCharacterArrayRecord : public CArrayRecord
{
private:
/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CCharacterArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);
};

/////////////////////////////////////////////////////////////////////////////
// CStyleArrayRecord

class CStyleArrayRecord : public CArrayRecord
{
protected:
/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CStyleArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

	struct
	{
		STYLE_INDEX m_lNextStyleChange;
	} m_Record2;

public:

	// Allocate a new style change.
	CTextStyleChange* NewStyleChange(STYLE_INDEX* plChange);

	// Get an existing style change
	CTextStyleChange* GetStyleChange(STYLE_INDEX lChange);

	// Get an existing style change with intention to modify it.
	CTextStyleChange* ModifyStyleChange(STYLE_INDEX lChange);

	// Delete an existing style change.
	void DeleteStyleChange(STYLE_INDEX lChange);

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

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
// CParagraphArrayRecord

class CParagraphArrayRecord : public CArrayRecord
{
private:
/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CParagraphArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);
};

#endif					// #ifndef __TEXTREC_H__
