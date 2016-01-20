/*
// $Workfile: FFLDFILE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/FFLDFILE.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:24   Fred
// Initial revision.
// 
//    Rev 1.1   14 May 1997 10:16:44   Jay
// Changes for 16-bit file handle problem.
// 
//    Rev 1.0   14 Mar 1996 13:33:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:26   FRED
// Initial revision.
// 
//    Rev 1.0   07 Aug 1995 07:48:40   FRED
// Standard fill-in fields.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "ffldfile.h"
#include "file.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFillinFieldFile

CFillinFieldFile::CFillinFieldFile(void)
{
}

CFillinFieldFile::~CFillinFieldFile(void)
{
}

BOOL CFillinFieldFile::Init(LPCSTR lpszFileName, CStringArray* pArray)
{
	BOOL fResult = FALSE;

	m_pFields = pArray;

	ReadOnlyFile File(lpszFileName);
	File.set_read_buffer(2048);

	TRY
	{
		CFillinFieldFileParser Parser(&File, m_pFields);
		Parser.Parse(CFillinFieldFileParser::ParseStates, CFillinFieldFileParser::TokenLookup);
		fResult = TRUE;
	}
	END_TRY
		
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CFillinFieldFileParser

CFillinFieldFileParser::CFillinFieldFileParser(StorageFile* pWarpFile, CStringArray* pArray) :
	CParser(TRUE, FALSE)
{
	m_pFile = pWarpFile;
	m_pArray = pArray;
}

CFillinFieldFileParser::~CFillinFieldFileParser()
{
}

int CFillinFieldFileParser::NextChar(void)
{
	int nChar = 0;

	TRY
	{
		for (;;)
		{
			if (m_pFile->read(&nChar, 1) != ERRORCODE_None)
			{
				nChar = cspEOF;
			}
			// Skip CRs.
			if (nChar != '\r')
			{
				break;
			}
		}
	}
	CATCH (CFileException, e)
	{
	/* We only worry about EOF. */
		if (e->m_cause == CFileException::endOfFile)
		{
			nChar = cspEOF;
		}
		else
		{
			THROW_LAST();
		}
	}
	END_CATCH

	return nChar;
}

void CFillinFieldFileParser::UngetChar(int nChar)
{
	m_pFile->seek(-1, ST_DEV_SEEK_CUR);
}

CFillinFieldFileParser::PARSE_STATE CFillinFieldFileParser::ParseStates[] =
{

/*  0 */		{ TOKEN_BeginString,	(STATE_PROCESSOR)HandleName,			2,					1					},
/*  1 */		{ TOKEN_EOF,			NULL,											PARSE_ACCEPT,	PARSE_REJECT	},
/*  2 */		{ TOKEN_EndEntry,		(STATE_PROCESSOR)HandleEndEntry,		0,					PARSE_REJECT	}

};

TOKEN_LOOKUP CFillinFieldFileParser::TokenLookup[] =
{
	{ TOKEN_BeginString,	"\""	},
	{ TOKEN_LineComment,	"//"	},
	{ TOKEN_EndEntry,		";"	},

	{ TOKEN_EndOfList				}
};

void CFillinFieldFileParser::HandleString(void)
{
	int nChar;

/*
// Skip to end of the string. We currently don't care what this is.
*/

	int i = 0;

	while ((nChar = NextStreamChar()) != '"')
	{
	/* See if we hit end of file! */
		if (nChar == cspEOF)
		{
			ThrowParseException(CParseException::unexpected_end_of_file);
		}

		if (i == MAX_TOKEN_SIZE-1)
		{
			ThrowParseException(string_too_long);
		}

		m_cbString[i++] = nChar;
	}

	m_cbString[i] = '\0';

/* Pull in the next token. */

	Match();
}

void CFillinFieldFileParser::HandleName(void)
{
/* Parse the name. This does a Match(). */

	HandleString();
	if (strlen(m_cbString) > 0)
	{
		m_pArray->Add(m_cbString);
	}
}

void CFillinFieldFileParser::HandleEndEntry(void)
{
	Match();
}

