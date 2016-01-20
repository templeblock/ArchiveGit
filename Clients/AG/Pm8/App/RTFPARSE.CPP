/*
// Rich Text Format (RTF) parser
//
// $Header: /PM8/App/RTFPARSE.CPP 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/RTFPARSE.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:18   Fred
// Initial revision.
// 
//    Rev 1.5   20 Jul 1997 10:24:26   Jay
// Underlining had the wrong command in the table.
// 
//    Rev 1.4   13 Jan 1997 15:26:18   Jay
// 16-bit now compiles.
// 
//    Rev 1.3   23 Dec 1996 17:01:14   Jay
// Better RTF paste and text paste.
// 
//    Rev 1.2   18 Dec 1996 09:10:38   Jay
// More RTF parsing
// 
//    Rev 1.1   16 Dec 1996 13:02:12   Jay
// Round 2
// 
//    Rev 1.0   13 Dec 1996 13:13:18   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "rtfparse.h"

#ifndef WIN32
#include <ctype.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTFException

IMPLEMENT_DYNAMIC(CRTFException, CException);

void ThrowRTFException(int nError)
{
	THROW(new CRTFException(nError));
}

/////////////////////////////////////////////////////////////////////////////
// CRTFFontEntry

CRTFFontEntry::CRTFFontEntry()
{
	Reset();
}

void CRTFFontEntry::Reset(void)
{
	m_csName.Empty();
	m_nFontFamily = 0;
	m_nCharset = fnil;
	m_nPitch = pdefault;
}

/////////////////////////////////////////////////////////////////////////////
// CRTFColorEntry

CRTFColorEntry::CRTFColorEntry()
{
	Reset();
}

void CRTFColorEntry::Reset(void)
{
	m_nRed = m_nGreen = m_nBlue = -1;
}

BOOL CRTFColorEntry::IsDefined(void)
{
	return
			m_nRed != -1
		&& m_nGreen != -1
		&& m_nBlue != -1;
}

void CRTFColorEntry::SetDefault(int nIndex)
{
	m_nRed = m_nGreen = m_nBlue = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CRTFTab

CRTFTab::CRTFTab()
{
	Reset();
}

void CRTFTab::Reset(void)
{
	m_cLeader = 0;				// No leader
	m_nType = TAB_left;		// Defaults to left
}

/////////////////////////////////////////////////////////////////////////////
// CRTFParser

//
// Constructor.
//

CRTFParser::CRTFParser(CRTFSource* pSource, CRTFDestination* pDestination /*=NULL*/)
{
	ASSERT(pSource != NULL);
	m_pSource = pSource;
	m_pDestination = pDestination;

   m_pSave = NULL;
	m_cGroup = 0;
	m_rds = rdsNorm;
	m_ris = risNorm;
	m_pProps = m_PropsNorm;
	m_pSyms = m_SymsNorm;

	memset(&m_State.m_chp, 0, sizeof(m_State.m_chp));
	memset(&m_State.m_pap, 0, sizeof(m_State.m_pap));
	memset(&m_State.m_sep, 0, sizeof(m_State.m_sep));
	memset(&m_State.m_dop, 0, sizeof(m_State.m_dop));

	m_State.m_wChanged = All_Changed;

	m_cbBin = 0;
	m_lParam = 0;
	m_fSkipDestIfUnk = FALSE;
}

//
// Destructor.
//

CRTFParser::~CRTFParser()
{
	while (m_pSave != NULL)
	{
		SAVE* pSave = m_pSave;
		m_pSave = pSave->pNext;
		delete pSave;
	}

	int i;
	// Delete all font entries.
	for (i = 0; i < m_State.m_FontTable.GetSize(); i++)
	{
		delete (CRTFFontEntry*)m_State.m_FontTable.GetAt(i);
	}

	// Delete all color entries.
	for (i = 0; i < m_State.m_ColorTable.GetSize(); i++)
	{
		delete (CRTFColorEntry*)m_State.m_ColorTable.GetAt(i);
	}
}

//
// Main parser entry.
//

void CRTFParser::Parse(void)
{
	int ch;
	int cNibble = 2;
	int b = 0;
   while ((ch = m_pSource->GetChar()) != CRTFSource::RTFEOF)
	{
		if (m_cGroup < 0)
		{
			ThrowRTFException(CRTFException::ecStackUnderflow);
		}
		if (m_ris == risBin)          // if we are parsing binary data, handle it directly
		{
			ParseChar(ch);
		}
		else
		{
			switch (ch)
			{
				case '{':
				{
					PushRtfState();
					break;
				}
				case '}':
				{
					PopRtfState();
					break;
				}
				case '\\':
				{
					ParseRtfKeyword();
					break;
				}
				case 0x0d:
				case 0x0a:          // cr and lf are noise characters...
				{
					break;
				}
				default:
				{
					if (m_ris == risNorm)
					{
						ParseChar(ch);
					}
					else
					{               // parsing hex data
						if (m_ris != risHex)
						{
							ThrowRTFException(CRTFException::ecAssertion);
						}
						b <<= 4;
						if (isdigit(ch))
						{
							b += (char) ch - '0';
						}
						else
						{
							if (islower(ch))
							{
								if (ch < 'a' || ch > 'f')
								{
									ThrowRTFException(CRTFException::ecInvalidHex);
								}
								b += 0x0a + (char) ch - 'a';
							}
							else
							{
								if (ch < 'A' || ch > 'F')
								{
									ThrowRTFException(CRTFException::ecInvalidHex);
								}
								b += 0x0a + (char) ch - 'A';
							}
						}
						cNibble--;
						if (!cNibble)
						{
							ParseChar(b);
							cNibble = 2;
							b = 0;
							m_ris = risNorm;
						}
					}                   // end else (ris != risNorm)
					break;
				}
			}       // switch
		}           // else (m_ris != risBin)
	}               // while
	if (m_cGroup < 0)
	{
		ThrowRTFException(CRTFException::ecStackUnderflow);
	}
	if (m_cGroup > 0)
	{
		ThrowRTFException(CRTFException::ecUnmatchedBrace);
	}

	// Flush the destination if we have one.
	if (m_pDestination != NULL)
	{
		m_pDestination->Flush();
	}
}

//
// %%Function: PushRtfState
//
// Save relevant info on a linked list of SAVE structures.
//

void CRTFParser::PushRtfState(void)
{
	SAVE *pSaveNew = NULL;
	TRY
	{
		pSaveNew = new SAVE;
	}
	END_TRY

	if (pSaveNew == NULL)
	{
		ThrowRTFException(CRTFException::ecStackOverflow);
	}

	pSaveNew->pNext = m_pSave;
	pSaveNew->chp = m_State.m_chp;
	pSaveNew->pap = m_State.m_pap;
	pSaveNew->sep = m_State.m_sep;
	pSaveNew->dop = m_State.m_dop;
	pSaveNew->rds = m_rds;
	pSaveNew->ris = m_ris;
	pSaveNew->m_pProps = m_pProps;
	pSaveNew->m_pSyms = m_pSyms;

	m_ris = risNorm;
	m_pSave = pSaveNew;
	m_cGroup++;
}

//
// %%Function: PopRtfState
//
// If we're ending a destination (i.e., the destination is changing),
// call EndGroupAction.
// Always restore relevant info from the top of the SAVE list.
//

void CRTFParser::PopRtfState(void)
{
	if (!m_pSave)
	{
		ThrowRTFException(CRTFException::ecStackUnderflow);
	}

	if (m_rds != m_pSave->rds)
	{
		EndGroupAction(m_rds);
	}
	m_State.m_chp = m_pSave->chp;
	m_State.m_pap = m_pSave->pap;
	m_State.m_sep = m_pSave->sep;
	m_State.m_dop = m_pSave->dop;
	m_rds = m_pSave->rds;
	m_ris = m_pSave->ris;
	m_pProps = m_pSave->m_pProps;
	m_pSyms = m_pSave->m_pSyms;

	m_State.m_wChanged = All_Changed;

	SAVE *pSaveOld = m_pSave;

	m_pSave = m_pSave->pNext;
	m_cGroup--;
	delete pSaveOld;
}

//
// %%Function: ParseRtfKeyword
//
// Step 2:
// get a control word (and it's associated value) and
// call TranslateKeyword to dispatch the control.
//

void CRTFParser::ParseRtfKeyword(void)
{
	int ch;
	char fParam = FALSE;
	char fNeg = FALSE;
	int param = 0;
	char *pch;
	char szKeyword[30];
	char szParameter[20];

	szKeyword[0] = '\0';
	szParameter[0] = '\0';
   if ((ch = m_pSource->GetChar()) == CRTFSource::RTFEOF)
	{
		ThrowRTFException(CRTFException::ecEndOfFile);
	}
	if (!isalpha(ch))           // a control symbol; no \delimiter.
	{
		szKeyword[0] = (char) ch;
		szKeyword[1] = '\0';
		TranslateKeyword(szKeyword, 0, fParam);
      return;
	}
	for (pch = szKeyword; isalpha(ch); ch = m_pSource->GetChar())
	{
		*pch++ = (char) ch;
	}
	*pch = '\0';
	if (ch == '-')
	{
		fNeg = TRUE;
      if ((ch = m_pSource->GetChar()) == CRTFSource::RTFEOF)
		{
			ThrowRTFException(CRTFException::ecEndOfFile);
		}
	}

	if (isdigit(ch))
	{
		fParam = TRUE;         // a digit after the control means we have a parameter
		for (pch = szParameter; isdigit(ch); ch = m_pSource->GetChar())
		{
			*pch++ = (char) ch;
		}
		*pch = '\0';
		param = atoi(szParameter);
		m_lParam = atol(szParameter);
		if (fNeg)
		{
			param = -param;
			m_lParam = -m_lParam;
		}
	}
	if (ch != ' ')
	{
		m_pSource->UnGetChar(ch);
	}
	TranslateKeyword(szKeyword, param, fParam);
}

//
// %%Function: ParseChar
//
// Route the character to the appropriate destination stream.
//

void CRTFParser::ParseChar(int ch)
{
	if (m_ris == risBin && --m_cbBin <= 0)
	{
		m_ris = risNorm;
	}

	switch (m_rds)
	{
		case rdsSkip:
		{
			// Toss this character.
			break;
		}
		case rdsNorm:
		{
			// Output a character.  Properties are valid at this point.
			PrintChar(ch);
			break;
		}
		case rdsColorTbl:
		{
			if (ch == ';')
			{
				if (!m_ColorEntry.IsDefined())
				{
					m_ColorEntry.SetDefault(m_State.m_ColorTable.GetSize());
				}

            CRTFColorEntry* pEntry = NULL;

				TRY
				{
					pEntry = new CRTFColorEntry;
					*pEntry = m_ColorEntry;

					// Set the new color entry.
					m_State.m_ColorTable.Add(pEntry);
					TRACE("Added color %d, %d, %d (%d)\n",
							pEntry->m_nRed,
							pEntry->m_nGreen,
							pEntry->m_nBlue,
							m_State.m_ColorTable.GetSize()-1);
				}
				CATCH_ALL(e)
				{
					delete pEntry;
					pEntry = NULL;
				}
				END_CATCH_ALL
				// Reset for next color entry.
				m_ColorEntry.Reset();
			}
			else
			{
				ASSERT(FALSE);
			}

			break;
		}
		case rdsFontTbl:
		{
			if (ch == ';')
			{
				// End of font.
				CRTFFontEntry* pEntry = NULL;
				TRY
				{
					pEntry = new CRTFFontEntry;
					*pEntry = m_FontEntry;
					// We should not have a font at this slot already.
					// Check anyway.
					if (pEntry->m_nIndex < m_State.m_FontTable.GetSize())
					{
						CRTFFontEntry* pOldEntry = (CRTFFontEntry*)m_State.m_FontTable.GetAt(pEntry->m_nIndex);
						ASSERT(pOldEntry == NULL);
						delete pOldEntry;
					}
					// Set the new font entry.
					m_State.m_FontTable.SetAtGrow(pEntry->m_nIndex, pEntry);
					TRACE("Added font %s (%d)\n", (LPCSTR)pEntry->m_csName, pEntry->m_nIndex);
				}
				CATCH_ALL(e)
				{
					delete pEntry;
					pEntry = NULL;
				}
				END_CATCH_ALL
				// Reset for next font entry.
				m_FontEntry.Reset();
			}
			else
			{
				// Add the character to the name.
				m_FontEntry.m_csName += ch;
			}
			break;
		}
		default:
		{
			// handle other destinations....
			break;
		}
	}
}

//
// %%Function: PrintChar
//
// Send a character to the output file.
//

void CRTFParser::PrintChar(int ch)
{
	// If we have a destination, output the character.
	if (m_pDestination != NULL)
	{
		// Put the character with the current state.
		m_pDestination->PutChar(ch, m_State);

		// Nothing has been changed as of now.
		m_State.m_wChanged = None_Changed;
	}
}

// RTF parser tables

// "Normal" property descriptions
CRTFParser::PROP CRTFParser::m_PropsNorm[CRTFParser::ipropMax] =
{
    actnWord,   propChp,    offsetof(CHP, nForeColor),  // ipropForeColor
    actnWord,   propChp,    offsetof(CHP, nBackColor),  // ipropBackColor
    actnWord,   propChp,    offsetof(CHP, nFont),       // ipropFont
    actnWord,   propChp,    offsetof(CHP, nSize),       // ipropFontSize
    actnByte,   propChp,    offsetof(CHP, fBold),       // ipropBold
    actnByte,   propChp,    offsetof(CHP, fItalic),     // ipropItalic
    actnByte,   propChp,    offsetof(CHP, fUnderline),  // ipropUnderline
    actnWord,   propPap,    offsetof(PAP, xaLeft),      // ipropLeftInd
    actnWord,   propPap,    offsetof(PAP, xaRight),     // ipropRightInd
    actnWord,   propPap,    offsetof(PAP, xaFirst),     // ipropFirstInd
    actnWord,   propPap,    offsetof(PAP, xaSpaceBefore), // ipropSpaceBefore
    actnWord,   propPap,    offsetof(PAP, xaSpaceAfter),  // ipropSpaceAfter
    actnWord,   propPap,    offsetof(PAP, xaLineSpacing), // ipropLineSpacing
    actnWord,   propSep,    offsetof(SEP, cCols),       // ipropCols
    actnWord,   propSep,    offsetof(SEP, xaPgn),       // ipropPgnX
    actnWord,   propSep,    offsetof(SEP, yaPgn),       // ipropPgnY
    actnWord,   propDop,    offsetof(DOP, nDefaultFont), // ipropDefaultFont
    actnWord,   propDop,    offsetof(DOP, xaPage),      // ipropXaPage
    actnWord,   propDop,    offsetof(DOP, yaPage),      // ipropYaPage
    actnWord,   propDop,    offsetof(DOP, xaLeft),      // ipropXaLeft
    actnWord,   propDop,    offsetof(DOP, xaRight),     // ipropXaRight
    actnWord,   propDop,    offsetof(DOP, yaTop),       // ipropYaTop
    actnWord,   propDop,    offsetof(DOP, yaBottom),    // ipropYaBottom
    actnWord,   propDop,    offsetof(DOP, pgnStart),    // ipropPgnStart
    actnByte,   propSep,    offsetof(SEP, sbk),         // ipropSbk
    actnByte,   propSep,    offsetof(SEP, pgnFormat),   // ipropPgnFormat
    actnByte,   propDop,    offsetof(DOP, fFacingp),    // ipropFacingp
    actnByte,   propDop,    offsetof(DOP, fLandscape),  // ipropLandscape
    actnByte,   propPap,    offsetof(PAP, just),        // ipropJust
    actnSpec,   propPap,    0,                          // ipropPard
    actnSpec,   propChp,    0,                          // ipropPlain
    actnSpec,   propSep,    0,                          // ipropSectd
    actnWord,   propTab,    offsetof(CRTFTab, m_nType),   // ipropTabType
    actnByte,   propTab,    offsetof(CRTFTab, m_cLeader), // ipropTabLeader
};

// "Normal" keyword descriptions
CRTFParser::SYM CRTFParser::m_SymsNorm[] =
{
//	keyword			dflt						fPassDflt   kwd		idx
	"par",			0,								FALSE,	kwdChar,	0x0a,
	"line",			0,								FALSE,	kwdChar,	0x0a,
	"plain",			0,								FALSE,	kwdProp,	ipropPlain,
	"pard",			0,								FALSE,	kwdProp,	ipropPard,
	"sectd",			0,								FALSE,	kwdProp,	ipropSectd,
	"cf",				0,								FALSE,	kwdProp,	ipropForeColor,
	"cb",				0,								FALSE,	kwdProp,	ipropBackColor,
	"f",				0,								FALSE,	kwdProp,	ipropFont,
	"fs",				24,							FALSE,	kwdProp,	ipropFontSize,
	"b",				1,								FALSE,	kwdProp,	ipropBold,
	"ul",				1,								FALSE,	kwdProp,	ipropUnderline,
	"i",				1,								FALSE,	kwdProp,	ipropItalic,
	"li",				0,								FALSE,	kwdProp,	ipropLeftInd,
	"ri",				0,								FALSE,	kwdProp,	ipropRightInd,
	"fi",				0,								FALSE,	kwdProp,	ipropFirstInd,
	"sb",				0,								FALSE,	kwdProp,	ipropSpaceBefore,
	"sa",				0,								FALSE,	kwdProp,	ipropSpaceAfter,
	"sl",				0,								FALSE,	kwdProp,	ipropLineSpacing,
	"cols",			1,								FALSE,	kwdProp,	ipropCols,
	"sbknone",		sbkNon,						TRUE,		kwdProp,	ipropSbk,
	"sbkcol",		sbkCol,						TRUE,		kwdProp,	ipropSbk,
	"sbkeven",		sbkEvn,						TRUE,		kwdProp,	ipropSbk,
	"sbkodd",		sbkOdd,						TRUE,		kwdProp,	ipropSbk,
	"sbkpage",		sbkPg,						TRUE,		kwdProp,	ipropSbk,
	"pgnx",			0,								FALSE,	kwdProp,	ipropPgnX,
	"pgny",			0,								FALSE,	kwdProp,	ipropPgnY,
	"pgndec",		pgDec,						TRUE,		kwdProp,	ipropPgnFormat,
	"pgnucrm",		pgURom,						TRUE,		kwdProp,	ipropPgnFormat,
	"pgnlcrm",		pgLRom,						TRUE,		kwdProp,	ipropPgnFormat,
	"pgnucltr",		pgULtr,						TRUE,		kwdProp,	ipropPgnFormat,
	"pgnlcltr",		pgLLtr,						TRUE,		kwdProp,	ipropPgnFormat,
	"qc",				justC,						TRUE,		kwdProp,	ipropJust,
	"ql",				justL,						TRUE,		kwdProp,	ipropJust,
	"qr",				justR,						TRUE,		kwdProp,	ipropJust,
	"qj",				justF,						TRUE,		kwdProp,	ipropJust,
	"deff",			0,								FALSE,	kwdProp,	ipropDefaultFont,
	"paperw",		12240,						FALSE,	kwdProp,	ipropXaPage,
	"paperh",		15480,						FALSE,	kwdProp,	ipropYaPage,
	"margl",			1800,							FALSE,	kwdProp,	ipropXaLeft,
	"margr",			1800,							FALSE,	kwdProp,	ipropXaRight,
	"margt",			1440,							FALSE,	kwdProp,	ipropYaTop,
	"margb",			1440,							FALSE,	kwdProp,	ipropYaBottom,
	"pgnstart",		1,								TRUE,		kwdProp,	ipropPgnStart,
	"facingp",		1,								TRUE,		kwdProp,	ipropFacingp,
	"landscape",	1,								TRUE,		kwdProp,	ipropLandscape,
	"tqr",			CRTFTab::TAB_right,		TRUE,		kwdProp,	ipropTabType,
	"tqc",			CRTFTab::TAB_center,		TRUE,		kwdProp,	ipropTabType,
	"tqdec",			CRTFTab::TAB_decimal,	TRUE,		kwdProp,	ipropTabType,
	"tldot",			'.',							TRUE,		kwdProp,	ipropTabLeader,
	"tlhyph",		'-',							TRUE,		kwdProp,	ipropTabLeader,
	"tlul",			'_',							TRUE,		kwdProp,	ipropTabLeader,
	"tlth",			'-',							TRUE,		kwdProp,	ipropTabLeader,
	"tleq",			'=',							TRUE,		kwdProp,	ipropTabLeader,
	"tx",				0,								FALSE,	kwdSpec,	ipfnTab,
	"tb",				0,								FALSE,	kwdSpec,	ipfnTab,
	"bin",			0,								FALSE,	kwdSpec,	ipfnBin,
	"*",				0,								FALSE,	kwdSpec,	ipfnSkipDest,
	"'",				0,								FALSE,	kwdSpec,	ipfnHex,
	"author",		0,								FALSE,	kwdDest,	idestSkip,
	"buptim",		0,								FALSE,	kwdDest,	idestSkip,
	"colortbl",		0,								FALSE,	kwdDest,	idestColorTbl,
	"comment",		0,								FALSE,	kwdDest,	idestSkip,
	"creatim",		0,								FALSE,	kwdDest,	idestSkip,
	"doccomm",		0,								FALSE,	kwdDest,	idestSkip,
	"fonttbl",		0,								FALSE,	kwdDest,	idestFontTbl,
	"falt",			0,								FALSE,	kwdDest,	idestSkip,
	"fontemb",		0,								FALSE,	kwdDest,	idestSkip,
	"footer",		0,								FALSE,	kwdDest,	idestSkip,
	"footerf",		0,								FALSE,	kwdDest,	idestSkip,
	"footerl",		0,								FALSE,	kwdDest,	idestSkip,
	"footerr",		0,								FALSE,	kwdDest,	idestSkip,
	"footnote",		0,								FALSE,	kwdDest,	idestSkip,
	"ftncn",			0,								FALSE,	kwdDest,	idestSkip,
	"ftnsep",		0,								FALSE,	kwdDest,	idestSkip,
	"ftnsepc",		0,								FALSE,	kwdDest,	idestSkip,
	"header",		0,								FALSE,	kwdDest,	idestSkip,
	"headerf",		0,								FALSE,	kwdDest,	idestSkip,
	"headerl",		0,								FALSE,	kwdDest,	idestSkip,
	"headerr",		0,								FALSE,	kwdDest,	idestSkip,
	"info",			0,								FALSE,	kwdDest,	idestSkip,
	"keywords",		0,								FALSE,	kwdDest,	idestSkip,
	"operator",		0,								FALSE,	kwdDest,	idestSkip,
	"pict",			0,								FALSE,	kwdDest,	idestSkip,
	"printim",		0,								FALSE,	kwdDest,	idestSkip,
	"private1",		0,								FALSE,	kwdDest,	idestSkip,
	"revtim",		0,								FALSE,	kwdDest,	idestSkip,
	"rxe",			0,								FALSE,	kwdDest,	idestSkip,
	"stylesheet",	0,								FALSE,	kwdDest,	idestSkip,
	"subject",		0,								FALSE,	kwdDest,	idestSkip,
	"tc",				0,								FALSE,	kwdDest,	idestSkip,
	"title",			0,								FALSE,	kwdDest,	idestSkip,
	"txe",			0,								FALSE,	kwdDest,	idestSkip,
	"xe",				0,								FALSE,	kwdDest,	idestSkip,
	"\0x0a",			0,								FALSE,	kwdChar,	0x0a,
	"\0x0d",			0,								FALSE,	kwdChar,	0x0a,
	"tab",			0,								FALSE,	kwdChar,	0x09,
	"cell",			0,								FALSE,	kwdChar,	0x09,
	"row",			0,								FALSE,	kwdChar,	0x0a,
	"lquote",		0,								FALSE,	kwdChar,	145,
	"rquote",		0,								FALSE,	kwdChar,	146,
	"ldblquote",	0,								FALSE,	kwdChar,	147,
	"rdblquote",	0,								FALSE,	kwdChar,	148,
	"bullet",		0,								FALSE,	kwdChar,	149,
	"endash",		0,								FALSE,	kwdChar,	150,
	"emdash",		0,								FALSE,	kwdChar,	151,
	"{",				0,								FALSE,	kwdChar,	'{',
	"}",				0,								FALSE,	kwdChar,	'}',
	"\\",				0,								FALSE,	kwdChar,	'\\',
	"",				0,								FALSE,	kwdEnd,	0
};

// "Font" property descriptions
CRTFParser::PROP CRTFParser::m_PropsFont[CRTFParser::ifntpropMax] =
{
    actnWord,   propFnt,    offsetof(CRTFFontEntry, m_nIndex),			// ifntpropIndex
    actnWord,   propFnt,    offsetof(CRTFFontEntry, m_nFontFamily),	// ifntpropFamily
    actnWord,   propFnt,    offsetof(CRTFFontEntry, m_nCharset),		// ifntpropCharset
    actnWord,   propFnt,    offsetof(CRTFFontEntry, m_nPitch),			// ifntpropPitch
};

// "Font" keyword descriptions
CRTFParser::SYM CRTFParser::m_SymsFont[] =
{
//  keyword			dflt								fPassDflt   kwd         idx
    "f",				0,									FALSE,		kwdProp,    ifntpropIndex,
    "fnil",			CRTFFontEntry::fnil,			TRUE,			kwdProp,    ifntpropFamily,
    "froman",		CRTFFontEntry::froman,		TRUE,			kwdProp,    ifntpropFamily,
    "fswiss",		CRTFFontEntry::fswiss,		TRUE,			kwdProp,    ifntpropFamily,
    "fmodern",		CRTFFontEntry::fmodern,		TRUE,			kwdProp,    ifntpropFamily,
    "fscript",		CRTFFontEntry::fscript,		TRUE,			kwdProp,    ifntpropFamily,
    "fdecor",		CRTFFontEntry::fdecor,		TRUE,			kwdProp,    ifntpropFamily,
    "ftech",		CRTFFontEntry::ftech,		TRUE,			kwdProp,    ifntpropFamily,
    "fbidi",		CRTFFontEntry::fbidi,		TRUE,			kwdProp,    ifntpropFamily,
    "fcharset",	0,									FALSE,		kwdProp,    ifntpropCharset,
    "fprq",			CRTFFontEntry::pdefault,	FALSE,		kwdProp,    ifntpropCharset,
    "*",				0,									FALSE,		kwdSpec,    ipfnSkipDest,
    "'",				0,									FALSE,		kwdSpec,    ipfnHex,
    "",				0,									FALSE,		kwdEnd,     0
};

// "Color" property descriptions
CRTFParser::PROP CRTFParser::m_PropsColor[CRTFParser::iclrpropMax] =
{
    actnWord,   propClr,    offsetof(CRTFColorEntry, m_nRed),			// ifntpropRed
    actnWord,   propClr,    offsetof(CRTFColorEntry, m_nGreen),		// ifntpropGreen
    actnWord,   propClr,    offsetof(CRTFColorEntry, m_nBlue),			// ifntpropBlue
};

// "Color" keyword descriptions
CRTFParser::SYM CRTFParser::m_SymsColor[] =
{
//  keyword			dflt								fPassDflt   kwd         idx
    "red",			0,									FALSE,		kwdProp,    iclrpropRed,
    "green",		0,									FALSE,		kwdProp,    iclrpropGreen,
    "blue",			0,									FALSE,		kwdProp,    iclrpropBlue,
    "*",				0,									FALSE,		kwdSpec,    ipfnSkipDest,
    "'",				0,									FALSE,		kwdSpec,    ipfnHex,
    "",				0,									FALSE,		kwdEnd,     0
};

//
// %%Function: ApplyPropChange
//
// Set the property identified by _iprop_ to the value _val_.
//
//

void CRTFParser::ApplyPropChange(int iprop, int val)
{
	char *pb;

	if (m_rds == rdsSkip)                 // If we are skipping text,
	{
		return;									// do not do anything.
	}

	switch (m_pProps[iprop].prop)
	{
		case propDop:
		{
			pb = (char *)&m_State.m_dop;
			m_State.m_wChanged |= DOP_Changed;
			break;
		}
		case propSep:
		{
			pb = (char *)&m_State.m_sep;
			m_State.m_wChanged |= SEP_Changed;
			break;
		}
		case propPap:
		{
			pb = (char *)&m_State.m_pap;
			m_State.m_wChanged |= PAP_Changed;
			break;
		}
		case propChp:
		{
			pb = (char *)&m_State.m_chp;
			m_State.m_wChanged |= CHP_Changed;
			break;
		}
		case propFnt:
		{
			pb = (char *)&m_FontEntry;
			break;
		}
		case propClr:
		{
			pb = (char *)&m_ColorEntry;
			break;
		}
		case propTab:
		{
			pb = (char *)&m_Tab;
			break;
		}
		default:
		{
			if (m_pProps[iprop].actn != actnSpec)
			{
            ThrowRTFException(CRTFException::ecBadTable);
			}
			break;
		}
	}

	switch (m_pProps[iprop].actn)
	{
		case actnByte:
		{
			pb[m_pProps[iprop].offset] = (unsigned char) val;
			break;
		}
		case actnWord:
		{
			(*(int *) (pb+m_pProps[iprop].offset)) = val;
			break;
		}
		case actnSpec:
		{
			ParseSpecialProperty(iprop, val);
			break;
		}
		default:
		{
			ThrowRTFException(CRTFException::ecBadTable);
		}
	}
}

//
// %%Function: ParseSpecialProperty
//
// Set a property that requires code to evaluate.
//

void CRTFParser::ParseSpecialProperty(int iprop, int val)
{
	switch (iprop)
	{
		case ipropPard:
		{
			memset(&m_State.m_pap, 0, sizeof(m_State.m_pap));
			m_State.m_wChanged |= PAP_Changed;

			if (m_pDestination != NULL)
			{
				m_pDestination->Pard(m_State);
			}
			break;
		}
		case ipropPlain:
		{
			memset(&m_State.m_chp, 0, sizeof(m_State.m_chp));
			m_State.m_chp.nFont = m_State.m_dop.nDefaultFont;
			m_State.m_chp.nSize = 28;
			m_State.m_wChanged |= CHP_Changed;

			if (m_pDestination != NULL)
			{
				m_pDestination->Plain(m_State);
			}
			break;
		}
		case ipropSectd:
		{
			memset(&m_State.m_sep, 0, sizeof(m_State.m_sep));
			m_State.m_wChanged |= SEP_Changed;

			break;
		}
		default:
		{
			ThrowRTFException(CRTFException::ecBadTable);
			break;
		}
	}
}

//
// %%Function: TranslateKeyword.
//
// Step 3.
// Search rgsymRtf for szKeyword and evaluate it appropriately.
//
// Inputs:
// szKeyword:   The RTF control to evaluate.
// param:       The parameter of the RTF control.
// fParam:      TRUE if the control had a parameter; (i.e., if param is valid)
//              FALSE if it did not.
//

void CRTFParser::TranslateKeyword(char *szKeyword, int param, BOOL fParam)
{
//	TRACE("TranslateKeyword: %s\n", szKeyword);

	int isym;

	// search for szKeyword in rgsymRtf

	for (isym = 0; m_pSyms[isym].kwd != kwdEnd; isym++)
	{
		if (strcmp(szKeyword, m_pSyms[isym].szKeyword) == 0)
		{
			break;
		}
	}
	if (m_pSyms[isym].kwd == kwdEnd)		// control word not found
	{
		if (m_fSkipDestIfUnk)				// if this is a new destination
		{
			TRACE("Skipping keyword: %s\n", szKeyword);
			m_rds = rdsSkip;					// skip the destination
		}
		// Else just discard it
		TRACE("Discarding keyword: %s\n", szKeyword);
		m_fSkipDestIfUnk = FALSE;
		return;
	}

	// found it!  use kwd and idx to determine what to do with it.

	m_fSkipDestIfUnk = FALSE;
	switch (m_pSyms[isym].kwd)
	{
		case kwdProp:
		{
			if (m_pSyms[isym].fPassDflt || !fParam)
			{
				param = m_pSyms[isym].dflt;
			}
			ApplyPropChange(m_pSyms[isym].idx, param);
			break;
		}
		case kwdChar:
		{
			ParseChar(m_pSyms[isym].idx);
			break;
		}
		case kwdDest:
		{
			ChangeDest(m_pSyms[isym].idx);
			break;
		}
		case kwdSpec:
		{
			ParseSpecialKeyword((IPFN)m_pSyms[isym].idx, param);
			break;
		}
		default:
		{
			ThrowRTFException(CRTFException::ecBadTable);
		}
	}
}

//
// %%Function: ChangeDest
//
// Change to the destination specified by idest.
// There's usually more to do here than this...
//

void CRTFParser::ChangeDest(int idest)
{
	if (m_rds == rdsSkip)             // if we're skipping text,
	{
		return;                // don't do anything
	}

	switch (idest)
	{
		case idestColorTbl:
		{
			m_rds = rdsColorTbl;
			m_pProps = m_PropsColor;
			m_pSyms = m_SymsColor;

			m_ColorEntry.Reset();
			break;
		}
		case idestFontTbl:
		{
			m_rds = rdsFontTbl;
			m_pProps = m_PropsFont;
			m_pSyms = m_SymsFont;

			m_FontEntry.Reset();
			break;
		}
		default:
		{
			m_rds = rdsSkip;              // when in doubt, skip it...
			break;
		}
	}
}

//
// %%Function: EndGroupAction
//
// The destination specified by m_rds is coming to a close.
// If there's any cleanup that needs to be done, do it now.
//

void CRTFParser::EndGroupAction(int rds)
{
}

//
// %%Function: ParseSpecialKeyword
//
// Evaluate an RTF control that needs special processing.
//

void CRTFParser::ParseSpecialKeyword(int ipfn, int param)
{
	if (m_rds == rdsSkip && ipfn != ipfnBin)  // if we're skipping, and it's not
	{
		return;                        // the \bin keyword, ignore it.
	}

	switch (ipfn)
	{
		case ipfnBin:
		{
			m_ris = risBin;
			m_cbBin = m_lParam;
			break;
		}
		case ipfnSkipDest:
		{
			m_fSkipDestIfUnk = TRUE;
			break;
		}
		case ipfnHex:
		{
			m_ris = risHex;
			break;
		}
		case ipfnTab:
		{
			FinishTab(param);
			break;
		}
		default:
		{
			ThrowRTFException(CRTFException::ecBadTable);
		}
	}
}

void CRTFParser::FinishTab(int param)
{
	m_Tab.m_nPosition = param;
	if (m_pDestination != NULL)
	{
		m_pDestination->SetTab(&m_Tab, m_State);
	}
	m_Tab.Reset();
}

/////////////////////////////////////////////////////////////////////////////
// CMemoryRTFSource

CMemoryRTFSource::CMemoryRTFSource(LPBYTE pData, DWORD dwSize)
{
	m_pData = pData;
	m_dwSize = dwSize;
	m_dwIndex = 0;
}

CMemoryRTFSource::~CMemoryRTFSource()
{
}

int CMemoryRTFSource::GetChar(void)
{
	if (m_dwIndex == m_dwSize || *m_pData == '\0')
	{
		return RTFEOF;
	}
	m_dwIndex++;
	return *m_pData++;
}

void CMemoryRTFSource::UnGetChar(int nChar)
{
	if (m_dwIndex == 0 || nChar != (int)*--m_pData)
	{
		ASSERT(FALSE);
		ThrowRTFException(CRTFException::ecAssertion);
	}

	m_dwIndex--;
}

