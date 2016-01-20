/*
// Rich Text Format (RTF) parser
//
// $Header: /PM8/App/RTFPARSE.H 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/RTFPARSE.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:48   Fred
// Initial revision.
// 
//    Rev 1.3   23 Dec 1996 17:01:16   Jay
// Better RTF paste and text paste.
// 
//    Rev 1.2   18 Dec 1996 09:10:38   Jay
// More RTF parsing
// 
//    Rev 1.1   16 Dec 1996 13:02:20   Jay
// Round 2
// 
//    Rev 1.0   13 Dec 1996 13:13:20   JAY
// Initial revision.
*/

class CRTFSource;
class CRTFDestination;
class CRTFParser;

#ifndef __RTFPARSE_H__
#define __RTFPARSE_H__

/////////////////////////////////////////////////////////////////////////////
// CRTFFontEntry
//   An entry in the font table.

class CRTFFontEntry
{
public:
	CRTFFontEntry();

	void Reset(void);

	enum FontFamily
	{
		fnil = 0,			// Unknown or default fonts
		froman = 1,			// Roman, proportionally spaced serif fonts (Times New Roman, Palatino)
		fswiss = 2,			// Swiss, proportionally spaced sans serif fonts (Arial)
		fmodern = 3,		// Fixed-pitch serif and sans serif fonts (Courier New, Pica)
		fscript = 4,		// Script fonts (Cursive)
		fdecor = 5,			// Decorative fonts (Old English)
		ftech = 6,			// Technical, symbol, and mathematical fonts (Symbol)
		fbidi = 7			// Arabic, Hebrew, or other bidirectional font (Miriam)
	};

	enum FontPitch
	{
		pdefault = 0,
		pfixed = 1,
		pvariable = 2
	};

	int m_nIndex;			// The index of this font.

	// Primary font mapping.
	CString m_csName;		// The name of the font.

	// Alternate font mapping.
	int m_nFontFamily;	// Font family
	int m_nCharset;		// Font character set
	int m_nPitch;			// Font pitch
};

/////////////////////////////////////////////////////////////////////////////
// CRTFColorEntry
//   An entry in the color table.

class CRTFColorEntry
{
public:
	CRTFColorEntry();
	void Reset(void);
	BOOL IsDefined(void);
	void SetDefault(int nIndex);

	int m_nRed;
	int m_nGreen;
	int m_nBlue;
};

/////////////////////////////////////////////////////////////////////////////
// CRTFTab
//   A tab filled out while parsing an RTF file.

class CRTFTab
{
public:
	CRTFTab();
	void Reset(void);

	// We order these to be the same as TabType in CTextTab (text.h)
	enum TabType
	{
		TAB_left,
		TAB_center,
		TAB_right,
		TAB_decimal,
	};

	char m_cLeader;				// Leader character (0 == none)
	int m_nType;					// Type of tab (see TabType above)
	int m_nPosition;				// Position of tab in twips
};

/////////////////////////////////////////////////////////////////////////////
// CRTFParser
//   The RTF Parser

class CRTFParser
{
public:
	CRTFParser(CRTFSource* pSource, CRTFDestination* pDestination = NULL);
	virtual ~CRTFParser();

	void Parse(void);

public:

	enum JUST {justL, justR, justC, justF };

	enum SBK {sbkNon, sbkCol, sbkEvn, sbkOdd, sbkPg};

	enum PGN {pgDec, pgURom, pgLRom, pgULtr, pgLLtr};

	// CHaracter Properties
	struct CHP
	{
		int nForeColor;				// \cf - Foreground color index
		int nBackColor;				// \cb - Background color index
		int nFont;						// \f  - Font index
		int nSize;						// \fs - Font size (in half points)
		char fBold;						// \b  - Bold
		char fUnderline;				// \u  - Underline
		char fItalic;					// \i  - Italic
	};

	// PAragraph Properties
	struct PAP
	{
		int xaLeft;						// \li - left indent in twips
		int xaRight;					// \ri - right indent in twips
		int xaFirst;					// \fi - first line indent in twips
		int xaSpaceBefore;			// \sb - space before paragraph
		int xaSpaceAfter;				// \sa - space after paragraph
		int xaLineSpacing;			// \sl - line spacing
		JUST just;						// \ql, \qc, \qr, \qj - justification
	};

	// SEction Properties
	struct SEP
	{
		int cCols;                  // number of columns
		SBK sbk;                    // section break type
		int xaPgn;                  // x position of page number in twips
		int yaPgn;                  // y position of page number in twips
		PGN pgnFormat;              // how the page number is formatted
	};

	// DOcument Properties
	struct DOP
	{
		int nDefaultFont;				 // default font index
		int xaPage;                 // page width in twips
		int yaPage;                 // page height in twips
		int xaLeft;                 // left margin in twips
		int yaTop;                  // top margin in twips
		int xaRight;                // right margin in twips
		int yaBottom;               // bottom margin in twips
		int pgnStart;               // starting page number in twips
		char fFacingp;              // facing pages enabled?
		char fLandscape;            // landscape or portrait??
	};

	// Current state. All we need to insert text.
	struct CRTFState
	{
		WORD m_wChanged;	// Have we changed (see enum below)?
		CHP m_chp;			// CHaracter Properties
		PAP m_pap;			// PAragraph Properties
		SEP m_sep;			// SEction Properties
		DOP m_dop;			// DOcument Properties
		CPtrArray m_FontTable;
		CPtrArray m_ColorTable;
	};

	enum
	{
		None_Changed	= 0,

		CHP_Changed		= 1,
		PAP_Changed		= 2,
		SEP_Changed		= 4,
		DOP_Changed		= 8,

		All_Changed		= (CHP_Changed | PAP_Changed | SEP_Changed | DOP_Changed)
	};

protected:

	// Rtf Destination State
	enum RDS
	{
		rdsNorm,							// Normal destination
		rdsSkip,							// Skip it!
		rdsColorTbl,					// Color table destination
		rdsFontTbl						// Font table destination
	};

	// Rtf Internal State
	enum RIS { risNorm, risBin, risHex };

	// What types of properties are there?
	enum IPROP
	{
		ipropForeColor, ipropBackColor,
		ipropFont, ipropFontSize,
		ipropBold, ipropItalic, ipropUnderline,
		ipropLeftInd, ipropRightInd, ipropFirstInd,
		ipropSpaceBefore, ipropSpaceAfter,
		ipropLineSpacing,
		ipropCols,
		ipropPgnX, ipropPgnY,
		ipropDefaultFont,
		ipropXaPage, ipropYaPage,
		ipropXaLeft, ipropXaRight,
		ipropYaTop, ipropYaBottom,
		ipropPgnStart,
		ipropSbk,
		ipropPgnFormat,
		ipropFacingp,
		ipropLandscape,
		ipropJust,
		ipropPard,
		ipropPlain,
		ipropSectd,
		ipropTabType, ipropTabLeader,
		ipropMax
	};

	enum IFNTPROP
	{
		ifntpropIndex,
		ifntpropFamily,
		ifntpropCharset,
		ifntpropPitch,
		ifntpropMax
	};

	enum ICLRPROP
	{
		iclrpropRed,
		iclrpropGreen,
		iclrpropBlue,
		iclrpropMax
	};

	enum ACTN {actnSpec, actnByte, actnWord};
	enum PROPTYPE {propChp, propPap, propSep, propDop, propFnt, propClr, propTab};

	struct PROP
	{
		ACTN actn;              // size of value
		PROPTYPE prop;          // structure containing value
		int  offset;            // offset of value from base of structure
	};

	enum IPFN
	{
		ipfnBin,
		ipfnHex,
		ipfnSkipDest,
		ipfnTab,
	};
	enum IDEST {idestPict, idestSkip, idestColorTbl, idestFontTbl };
	enum KWD {kwdChar, kwdDest, kwdProp, kwdSpec, kwdEnd};

	struct SYM
	{
		char *szKeyword;        // RTF keyword
		int  dflt;              // default value to use
		BOOL fPassDflt;         // TRUE to use default value from this table
		KWD  kwd;               // base action to take
		int  idx;               // index into property table if kwd == kwdProp
										// index into destination table if kwd == kwdDest
										// character to print if kwd == kwdChar
	};

	// property save structure
	struct SAVE
	{
		SAVE *pNext;         // next save
		CHP chp;
		PAP pap;
		SEP sep;
		DOP dop;
		RDS rds;
		RIS ris;
		PROP* m_pProps;
		SYM* m_pSyms;
	};

	// Functions
	void PushRtfState(void);
	void PopRtfState(void);
	void ParseRtfKeyword(void);
	void ParseChar(int c);
	void PrintChar(int ch);
	void ApplyPropChange(int iprop, int val);
	void ParseSpecialProperty(int iprop, int val);
	void TranslateKeyword(char *szKeyword, int param, BOOL fParam);
	void ChangeDest(int idest);
	void EndGroupAction(int rds);
	void ParseSpecialKeyword(int ipfn, int param);

	void FinishTab(int param);

	// Variable declarations

	CRTFSource* m_pSource;
	CRTFDestination* m_pDestination;

	int m_cGroup;
	RDS m_rds;			// Current destination state
	RIS m_ris;			// Current internal state
	PROP* m_pProps;	// Current properties
	SYM* m_pSyms;		// Current symbols

	CRTFState m_State;

	SAVE *m_pSave;
	long m_cbBin;
	long m_lParam;
	BOOL m_fSkipDestIfUnk;

	// Font table management.
	CRTFFontEntry m_FontEntry;

	// Color table management.
	CRTFColorEntry m_ColorEntry;

	// Current tab.
	CRTFTab m_Tab;

	static PROP m_PropsNorm[ipropMax];
	static SYM m_SymsNorm[];

	static PROP m_PropsFont[ifntpropMax];
	static SYM m_SymsFont[];

	static PROP m_PropsColor[iclrpropMax];
	static SYM m_SymsColor[];
};

// RTF Parse exception

class CRTFException : public CException
{
	DECLARE_DYNAMIC(CRTFException);
public:
// RTF parser error codes

	enum
	{
		ecOK = 0,					// Everything's fine!
		ecStackUnderflow = 1,	// Unmatched '}'
		ecStackOverflow = 2,		// Too many '{' -- memory exhausted
		ecUnmatchedBrace = 3,	// RTF ended during an open group.
		ecInvalidHex = 4,			// invalid hex character found in data
		ecBadTable = 5,			// RTF table (sym or prop) invalid
		ecAssertion = 6,			// Assertion failure
		ecEndOfFile = 7			// End of file reached while reading RTF
	};

	int m_nError;
	CRTFException(int nError)
		{ m_nError = nError; }
};

extern void ThrowRTFException(int nError);

/////////////////////////////////////////////////////////////////////////////
// CRTFSource
//   A source for reading RTF data.

class CRTFSource
{
public:
	virtual ~CRTFSource() {}

	virtual int GetChar(void) = 0;
	virtual void UnGetChar(int nChar) = 0;
	enum
	{
		RTFEOF = -1
	};
};

/////////////////////////////////////////////////////////////////////////////
// CRTFDestination
//   A destination for reading RTF data.

class CRTFDestination
{
public:
	virtual ~CRTFDestination() {}

	virtual void PutChar(int c, const CRTFParser::CRTFState& State) = 0;
	virtual void Pard(const CRTFParser::CRTFState& State) = 0;
	virtual void Plain(const CRTFParser::CRTFState& State) = 0;
	virtual void SetTab(const CRTFTab* pTab, const CRTFParser::CRTFState& State) = 0;
	virtual void Flush(void) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CMemoryRTFSource

class CMemoryRTFSource : public CRTFSource
{
public:
	CMemoryRTFSource(LPBYTE pData, DWORD dwSize);
	virtual ~CMemoryRTFSource();

	// Functions.
	virtual int GetChar(void);
	virtual void UnGetChar(int nChar);

	// Variable data.
#ifdef WIN32
	LPBYTE m_pData;
#else
	BYTE huge* m_pData;
#endif
	DWORD m_dwSize;
	DWORD m_dwIndex;
};

#endif
