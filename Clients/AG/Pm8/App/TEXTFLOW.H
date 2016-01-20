/*
// $Header: /PM8/App/TEXTFLOW.H 1     3/03/99 6:12p Gbeddow $
//
// Textflow definitions.
//
// $Log: /PM8/App/TEXTFLOW.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 4     2/11/99 11:55a Cschende
// Added code for Drop Caps implementation
// 
// 3     12/10/98 4:11p Cschende
// Added bullet, numbered list and multilevel list support in text
// 
// 2     12/04/97 2:24p Hforman
// CArray -> CVarArray
// 
//    Rev 1.0   14 Aug 1997 15:26:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:12   Fred
// Initial revision.
// 
//    Rev 1.19   05 Nov 1996 14:07:00   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.18   31 Jul 1996 18:21:12   Jay
// Fixes, etc!
// 
//    Rev 1.17   12 Jul 1996 16:48:24   Jay
// Text wrap
// 
//    Rev 1.16   09 Jul 1996 15:37:18   Jay
//  
// 
//    Rev 1.15   08 Jul 1996 17:51:38   Jay
// Linked frames, etc.
// 
//    Rev 1.14   28 Jun 1996 16:18:38   Jay
//  
// 
//    Rev 1.13   27 Jun 1996 13:27:48   Jay
//  
// 
//    Rev 1.12   24 Jun 1996 18:06:16   Jay
//  
// 
//    Rev 1.11   21 Jun 1996 14:30:32   Jay
// Stretch text, etc.
// 
//    Rev 1.10   20 Jun 1996 16:15:58   Jay
// Frame refresh offsets
// 
//    Rev 1.9   20 Jun 1996 14:59:52   Jay
//  
// 
//    Rev 1.8   19 Jun 1996 17:32:14   Jay
//  
// 
//    Rev 1.7   19 Jun 1996 13:45:54   Jay
//  
// 
//    Rev 1.6   18 Jun 1996 16:14:52   Jay
//  
// 
//    Rev 1.5   17 Jun 1996 13:19:48   Jay
//  
// 
//    Rev 1.4   17 Jun 1996 12:01:30   Jay
//  
// 
//    Rev 1.3   14 Jun 1996 17:05:44   Jay
//  
// 
//    Rev 1.2   13 Jun 1996 15:53:10   Jay
//  
// 
//    Rev 1.1   04 Jun 1996 17:38:58   Jay
//  
*/

#ifndef __TEXTFLOW_H__
#define __TEXTFLOW_H__

class CTextRecord;
class CFrameRecord;
class CFrameObject;
struct CTextState;

#include "text.h"
#include "carray.h"

/////////////////////////////////////////////////////////////////////////////
// CTextFlowExtent - a text flow extent
// This is a little structure that is stored in a CVarArray. It must NOT contain
// any virtual functions.

struct CTextFlowExtent
{
	PCOORD m_ExtentLeft;			// What is the left coordinate for this extent?
	PCOORD m_ExtentRight;		// What is the right coordinate for this extent?
	PCOORD m_UnusedWidth;		// How much space is left in this extent?
	int m_nFirstWord;				// First word in this extent (index into array)
	int m_nNumberOfWords;		// Number of words in this extent.
};

/////////////////////////////////////////////////////////////////////////////
// CWordIterator
// This class holds the variables we need to traverse all the words in the
// story. This is necessary because each frame has its own word array. We
// need to be able to easily move forward and backward in the virtual array
// that is the concatentation of all these arrays.

class CWordIterator
{
public:
	// Constructor.
	CWordIterator();

	// Destructor.
	~CWordIterator();

	// Initialize. Then we can begin iterating.
	void Initialize(void);

	// Make sure we have our current frame locked.
	void GetFrame(void);

	// Release the frame if we have one.
	void ReleaseFrame(void);

	// Cleanup any resources we may have allocated or locked.
	void Cleanup(void)
		{ ReleaseFrame(); }

	// Get the text word corresponding to the current position.
	CTextWord* GetWord(void);

	// Move to the next word.
	void NextWord(void);
	
	// Make sure we're pointing at a word.
	void LoadWord(void);
	
	PMGDatabase* m_pDatabase;
	DB_RECORD_NUMBER m_lFrame;
	CFrameRecord* m_pFrame;
	WORD_INDEX m_lWord;
	BOOL m_fCanAdvance;
};

/////////////////////////////////////////////////////////////////////////////
// CTextFlow - the class needed for interruptable text flow.
// This contains the original input parameters as well as anything generated
// in-process that is needed to continue on after an interruption.

class CTextFlow
{
public:
	// Constructors
	CTextFlow();
	CTextFlow(DB_RECORD_NUMBER lFrame,
				 PMGDatabase* pDatabase,
				 CHARACTER_INDEX lStartCharacter,
				 CHARACTER_INDEX lEndCharacter);

	CTextFlow(CFrameObject* pFrameObject,
				 CHARACTER_INDEX lStartCharacter,
				 CHARACTER_INDEX lEndCharacter);

	// Initialization functions.
	void Init(DB_RECORD_NUMBER lFrame,
				 PMGDatabase* pDatabase,
				 CHARACTER_INDEX lStartCharacter,
				 CHARACTER_INDEX lEndCharacter);

	void Init(CFrameObject* pFrameObject,
				 CHARACTER_INDEX lStartCharacter,
				 CHARACTER_INDEX lEndCharacter);

	// Destructor
	virtual ~CTextFlow();

	void FlowText(void);

	void Cleanup(void);

	// Status definitions and variables.
	enum
	{
		STATUS_Error = -1,
		STATUS_Initialize = 0,
		STATUS_Flowing,
		STATUS_Done,
		STATUS_Interrupted
	};

	enum
	{
		WORD_SOURCE_Characters,
		WORD_SOURCE_Words
	};

	enum
	{
		STRETCH_MODE_None,
		STRETCH_MODE_Flow,
		STRETCH_MODE_Size
	};

	void SetStatus(int nStatus, ERRORCODE nError = ERRORCODE_None)
		{ m_nStatus = nStatus; m_nError = nError; }

	int GetStatus(void) const
		{ return m_nStatus; }
	ERRORCODE GetError(void) const
		{ return m_nError; }

	void SetStretchMode(int nStretchMode)
		{ m_nStretchMode = nStretchMode; }
	VOID SetStretchDims(PPNT StretchDims)
		{ m_StretchDims = StretchDims; }

	void FlowingMacros(BOOL fFlowingMacros, CWordArray* pMacros /*=NULL*/)
		{ m_fFlowingMacros = fFlowingMacros; m_pMacros = pMacros; }
	BOOL FlowingMacros(void) const
		{ return m_fFlowingMacros; }

	// Input parameters.
	CFrameObject* m_pFrameObject;
	DB_RECORD_NUMBER m_lFrame;
	PMGDatabase* m_pDatabase;
	CHARACTER_INDEX m_lStartCharacter;
	CHARACTER_INDEX m_lEndCharacter;
	int m_nStretchMode;
	PPNT m_StretchDims;

protected:
	void CommonConstruct(void);

	void InitializeFlow(void);
	void FlowLine(void);

	// Flow frame management.
	PageRecord* m_pFramePage;
	void GetFrame(BOOL fSetState = FALSE, BOOL fResetFrame = FALSE);
	void ReleaseFrame(void);
	void InitializeFrame(void);
	void FinishFrame(BOOL fChopLines, BOOL fResetFrame = FALSE);

	void GetFrameObject(void);
	void ReleaseFramePage(void);

	// Source word management.
	CWordIterator m_Source;
	void InitializeSource(void);			// Initialize the word source.
	CTextWord* GetSourceWord(void);
	void NextSourceWord(void)
		{ m_Source.NextWord(); m_lCharacterInSourceWord = 0; }

	// Destination word management.
	CWordIterator m_Destination;
	void InitializeDestination(void);
	CTextWord* GetDestinationWord(void)
		{ return m_Destination.GetWord(); }
	void NextDestinationWord(void)
		{ m_Destination.NextWord(); }

	void FindLineOfCharacter(CHARACTER_INDEX lSearchCharacter);
	void BackupALine(void);

	CTextRecord* m_pTextRecord;
	CTextState* m_pTextState;
	CFrameRecord* m_pFrameRecord;
	CTextIterator m_Iterator;

	// Status.
	int m_nStatus;
	ERRORCODE m_nError;			// The error if m_Status == STATUS_Error

	// Convenience variables.
	PCOORD m_FrameWidth;
	PCOORD m_FrameHeight;
	WORD_COUNT m_lWordsToFlow;
	DWORD m_dwFrameFlags;

	// Vertical alignment variables.
	PCOORD m_FirstLineSpace;
	PCOORD m_LastLinesExtent;
	PCOORD m_CurrentLinesExtent;

	// Tab variables.
	int m_nCurrentTab;						// Index of last placed tab.
	CHARACTER_INDEX m_lCurrentTabEnd;	// First character past tab.
	CTextTab m_CurrentTab;					// The actual tab pending.
	PCOORD m_CurrentTabStartWidth;		// Initial width of last placed tab.
	PCOORD m_CurrentTabUnusedWidth;		// Unused width past tab.

	// Flags
	BOOL m_fSpaceExhausted;					// Have we run out of space to flow into?
	BOOL m_fWordsExhausted;					// Have we run out of words to flow?
	BOOL m_fLineComplete;
	BOOL m_fRestartLine;
	BOOL m_fPlacedEOTWord;
	BOOL m_fExtentClipped;
	BOOL m_fAddedWordsToLine;

	// Macro flow support.
	BOOL m_fFlowingMacros;					// Are we doing a special macro flow?
	CWordArray* m_pMacros;
	CHARACTER_COUNT m_lMacrosFlowed;
	CHARACTER_COUNT m_lMacrosToFlow;

	// Line traversal.
	LINE_INDEX m_lCurrentLine;
	WORD_INDEX m_lFirstWordInLine;
	CHARACTER_INDEX m_lFirstCharacterInLine;
	CVarArray m_WordArray;		// Array of words in the current extent.
	int m_nLineAlignment;
	CFixed m_SpaceAfter;

	int m_nLineInParagraph;	// what line in the paragarph are we on (added for drop caps)

	// Leading stuff extracted from the style.
	int m_nLeadingType;
	CFixed m_LeadingValue;

	// Line cell metrics. Useful for leading.
	PCOORD m_TopOfCell;
	PCOORD m_CellHeight;
	PCOORD m_CellAscend;
	PCOORD m_CellDescend;

	// Line metrics.
	PCOORD m_Baseline;
	PCOORD m_LineAscend;
	PCOORD m_LineDescend;

	// Drop Cap metrics & flags
	PCOORD m_DropWidth;
	PCOORD m_DropBaseline;
	int m_DropLines;
	int m_CurrentDropLine;
	CFixed m_DropSize;

	// Frame column variables.
	int m_nColumns;
	PBOX m_Margins;
	PCOORD m_ColumnWidth;
	PCOORD m_ColumnSpacing;

	// Current column variables.
	int m_nColumn;
	int m_nLineInColumn;
	PCOORD m_ColumnLeft;
	PCOORD m_ColumnRight;
	LINE_INDEX m_lFirstLineInColumn;		// Needed for vertical alignment.
	LINE_INDEX m_lLastLineInColumn;		// Needed for vertical alignment.

	// Character traversal.
	CHARACTER_INDEX m_lCharacter;

	// Word source variables.
	int m_nWordSource;
	CHARACTER_INDEX m_lCharacterInSourceWord;

	// Line restart variables.
	DB_RECORD_NUMBER m_lSourceFrameSave;
	WORD_INDEX m_lSourceWordSave;
	int m_nWordSourceSave;
	CHARACTER_INDEX m_lSourceCharacterSave;

	// Current word variables.
	CTextWord m_BaseWord;
	CTextWord m_CurrentWord;

	// Extent management.
	CVarArray m_ExtentArray;	// Array of extents in the current line.
	CTextFlowExtent* m_pTextFlowExtent;
	int m_nCurrentFlowExtent;

	// amount of space offset from the left margin for a bullet character
	PCOORD m_LeftOffset;

	// Column management.
	void InitializeColumn(BOOL fFirstTime);
	void FinishColumn(void);

	BOOL IsBaseWordEmpty(void) const
		{ return m_BaseWord.m_wType == (WORD)WORD_TYPE_unknown; }
	void SetBaseWordEmpty(void)
		{	m_BaseWord.m_wType = (WORD)WORD_TYPE_unknown; }

	void RestartLine(void);
	void StartLine(void);

	void FlowWord(void);
	void GetNextWord(void);
	void GetBaseWord(CHARACTER_COUNT nCharacters = 0);
	void ComputeFlowExtents(void);
	void SetCurrentFlowExtent(int nExtent);
	void GenerateTextFlowExtents(void);
	void AddTextFlowExtent(PCOORD Left, PCOORD Right);
	CTextFlowExtent* GetTextFlowExtent(int nExtent);

	void ComputeLeading(const CTextStyle& Style);

	void UpdateCurrentTab(void);
	BOOL WordFitsVertically(void);
	BOOL WordFitsHorizontally(void);
	void AddWordToLine(void);
	void EnlargeLine(void);
	BOOL BreakWord(void);
	void MoveWord(void);
	void ApplyLineAttributes(void);

	void AlignCenter(CTextFlowExtent* pExtent);
	void AlignRight(CTextFlowExtent* pExtent);
	void AlignJustify(CTextFlowExtent* pExtent);
	void AdjustExtent(CTextFlowExtent* pExtent, PCOORD Amount);
	PCOORD FlowExtentLeadingSpace(CTextFlowExtent* pExtent);
	PCOORD FlowExtentTrailingSpace(CTextFlowExtent* pExtent);

	BOOL MoveWordsToLine(void);
	void MoveToNextLine(void);

	void BackUpAWord(void);

	void AddRefreshExtent(PBOX* pRefresh, REFRESH_TYPE nRefreshType);
	// Refresh a line.
	void RefreshLine(CTextLine* pLine, REFRESH_TYPE nType);

	void DeleteDestinationWord(void);
	void InsertWord(WORD_INDEX lWord, CTextWord* pWord);

	static void MergeWords(CTextWord* pWord, CTextWord* pOtherWord);

	PCOORD ComputeLinesExtent(void);
	PCOORD ComputeTopLineOffset(void);
	BOOL UpdateVerticalAlignment(void);

	void SetDropDimensions();
	void UpdateLineInParagraph();
	void AdjustBaselines();


};

#endif					// #ifndef __TEXTFLOW_H__
