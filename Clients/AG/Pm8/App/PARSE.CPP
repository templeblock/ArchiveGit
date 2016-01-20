/*
// Compiler parse routines.
//
// May 22, 1990 - Initial definitions. (JN)
*/

#include	"stdafx.h"
#include	"parse.h"

#include	<ctype.h>
#include	<string.h>
#include	<stdlib.h>
#include	<stdio.h>

/*
// Definition variables.
*/

#if 0
PRIVATE SYMBOL_ENTRY_PTR defining = NULL;

PRIVATE BOOL start_definition(VOID);
PRIVATE BOOL end_definition(VOID);

PRIVATE PARSE_STATE
define_states[] =
{
/*  0 */		{ TOKEN_SYMBOL, start_definition, REL+1, REJECT},
/*  1 */		{ TOKEN_NUMBER, end_definition, ACCEPT, REJECT},
};
#endif

/*
// Constructor.
*/

CParser::CParser(BOOL fIgnoreCase, BOOL fParseExpressions, BOOL fNonAlphaSymbols)
{
	m_fIgnoreCase = fIgnoreCase;
	m_fParseExpressions = fParseExpressions;
	m_fNonAlphaSymbols = fNonAlphaSymbols;

	m_states = NULL;
	m_token_lookup = NULL;

	m_pSourceFile = NULL;

	Init();
}

/*
// Get the next character.
*/

int CParser::NextStreamChar()
{
	int nChar;

	if ((nChar = NextChar()) == '\n')
	{
	/* Bump the line count. */
		m_dwLineNo++;
	}

	return nChar;
}

/*
// Unget a character from the stream.
*/

void CParser::UngetStreamChar(int nChar)
{
	if (nChar == '\n')
	{
	/* Decrement the line count. */
		m_dwLineNo--;
	}

	UngetChar(nChar);
}

/*
// Look at the next character in the stream.
*/

int CParser::PeekStreamChar(void)
{
	int nChar;

	if ((nChar = NextChar()) != cspEOF)
	{
	/* Put back any non-EOF character. */
		UngetChar(nChar);
	}
	return nChar;
}

/*
// Lookup a token.
// NULL for token_lookup means search all lookups (watch for recursion).
*/

PARSE_TOKEN CParser::LookupSymbolToken(const char *pSymbol, const TOKEN_LOOKUP *token_lookup) const
{
/* Strange side-trip in case we ever want multiple lookups. */

	if (token_lookup == NULL)
	{
	/* Run through all the lookups. */
		return LookupSymbolToken(pSymbol, m_token_lookup);
	}

/*
// Look for the token in the token lookup.
*/

	for (int i = 0; token_lookup[i].m_tToken != TOKEN_EndOfList; i++)
	{
		int ret;
		if (m_fIgnoreCase)
		{
			ret = stricmp(pSymbol, token_lookup[i].m_pSymbol);
		}
		else
		{
			ret = strcmp(pSymbol, token_lookup[i].m_pSymbol);
		}

		if (ret == 0)
		{
			return token_lookup[i].m_tToken;
		}
	}
	return TOKEN_Unknown;
}

/*
// Lookup a symbol given a token.
// NULL for token_lookup means search all lookups (watch for recursion).
*/

const char *CParser::LookupTokenSymbol(PARSE_TOKEN tToken, const TOKEN_LOOKUP *token_lookup) const
{
/* Strange side-trip in case we ever want multiple lookups. */

	if (token_lookup == NULL)
	{
	/* Run through all the lookups. */
		return LookupTokenSymbol(tToken, m_token_lookup);
	}

/*
// Look for the token in the token lookup.
*/

	for (int i = 0; token_lookup[i].m_tToken != TOKEN_EndOfList; i++)
	{
		if ((tToken = token_lookup[i].m_tToken) == 0)
		{
			return token_lookup[i].m_pSymbol;
		}
	}
	return NULL;
}

/*
// Return the symbol for a token.
*/

char *CParser::TokenText(PARSE_TOKEN tToken, char *cbBuffer) const
{
	switch (tToken)
	{
		case TOKEN_EOF:
		{
			strcpy(cbBuffer, "EOF");
			break;
		}
		case TOKEN_Unknown:
		{
			strcpy(cbBuffer, m_cbTokenSymbol);
			break;
		}
		case TOKEN_Symbol:
		{
			sprintf(cbBuffer, "identifier '%s'", m_cbTokenSymbol);
			break;
		}
		case TOKEN_Number:
		{
			strcpy(cbBuffer, "NUMBER");
//			printf("[Number = %lx]\n", token_number);
			break;
		}
		case TOKEN_Anything:
		{
			strcpy(cbBuffer, "(Anything)");
			break;
		}
		default:
		{
			const char *pSymbol;

			if ((pSymbol = LookupTokenSymbol(tToken)) == NULL)
			{
			/* This is not a valid token. */
				ThrowParseException(CParseException::internal_error, "TokenText", "illegal token");
			}

			sprintf(cbBuffer, "'%s'", pSymbol);
		}
	}
	return cbBuffer;
}

/*
// Handle a definition (a #define).
*/

#if 0
PRIVATE BOOL
handle_definition()
{
	match();
	return(parse_states(define_states));
}
#endif

/*
// Get the statistics for a new file line.
*/

void CParser::HandleLineStatus(void)
{
	static char cbNewName[140];
	int nChar;

	if (NextToken() != TOKEN_Number)
	{
		ThrowParseException(CParseException::illegally_formed_line_status);
	}
	else
	{
	/* Got the line number. */

		DWORD dwNewLine = (DWORD)m_dTokenNumber;

	/* Skip white space to the file name. */

		do
		{
			nChar = NextStreamChar();
			if (nChar == '\n' || nChar == cspEOF)
			{
			/* Invalidly constructed line. */
				ThrowParseException(CParseException::illegally_formed_line_status);
			}
		} while (isspace(nChar));

	/* Now read the file name. */

		char *s = cbNewName;

	/* Collect... */

		do
		{
			*s++ = (char)nChar;

			nChar = NextStreamChar();
			if (nChar == cspEOF)
			{
			/* Hit end of file! Something is wrong. */
				ThrowParseException(CParseException::illegally_formed_line_status);
			}
		} while (!isspace(nChar));

		*s = '\0';

		while (nChar != '\n')
		{
			if ((nChar = NextStreamChar()) == cspEOF)
			{
				ThrowParseException(CParseException::illegally_formed_line_status);
			}
		}

	/* Success. Set the new variables. */

		SetNewFile(cbNewName, dwNewLine);
	}
}

/*
// Get the next non-white character.
*/

int CParser::NextNonWhite(void)
{
	int nChar;

	while (nChar = NextStreamChar(), isspace(nChar))
		;

	return nChar;
}

/*
// Parse an expression.
*/

double CParser::ParseExpression(int nChar)
{
	BOOL fNegative;
	BOOL fGotExpression;
	double dValue;

/* Did we just hit the end of file? */

	if (nChar == cspEOF)
	{
		ThrowParseException(CParseException::bad_expression);
	}

/* We have a non-white character. Start looking for a match. */

	fNegative = FALSE;
	fGotExpression = FALSE;

	for (; !isdigit(nChar); nChar = NextStreamChar())
	{
		if (isspace(nChar))
		{
			continue;
		}

		switch (nChar)
		{
			case '(':
			{
				if (!m_fParseExpressions)
				{
					ThrowParseException(CParseException::number_expected);
				}

			/* Parse the interior expression. */

				dValue = ParseExpression(NextStreamChar());

			/* Look for a closing ')'. */

				if ((nChar = NextStreamChar()) != ')')
				{
					if (nChar == cspEOF)
					{
						ThrowParseException(CParseException::bad_expression);
					}

					char *s = "A";

				/* A ')' is missing. */

					*s = (char)nChar;		/* Make a string from the character. */

					ThrowParseException(CParseException::unexpected_token, ")", s);
				}

			/* Handle any negative signs. */

				fGotExpression = TRUE;
				break;
			}
			case '-':
			{
				fNegative = !fNegative;
				break;
			}
			case '+':
			{
				break;
			}
			default:
			{
				char *s = "A";

			/* Not a digit! */
				*s = (char)nChar;

				ThrowParseException(CParseException::syntax_error, s);
			}
		}

		if (fGotExpression)
		{
			break;
		}
	}

/*
// Try to parse as a number.
*/

	if (!fGotExpression)
	{
		double dBase = 10.0;

	/* Parse as a number. */

		dValue = (double)(nChar-'0');
		dBase = 10.0;

	/* See if we should switch to hex base. */

		if (nChar == '0')
		{
		/* Check for "0x" or "0X" as flags for hex. */
			nChar = NextStreamChar();

			if (toupper(nChar) == 'X')
			{
			/* Parse as hex. */
				dBase = 16.0;
			/* Move to the next character. */
				nChar = NextStreamChar();
			}
		}
		else
		{
		/* Just get the next character. */
			nChar = NextStreamChar();
		}

		double dPlace = 1.0;

	/* Get the rest of the digits. */

		for (;; nChar = NextStreamChar())
		{
			if (isdigit(nChar))
			{
				nChar -= '0';
			}
			else if ((dBase == 16.0) && (isxdigit(nChar)))
			{
				nChar -= ('A' - 10);
			}
			else if (nChar == '.')
			{
			/* This is a decimal point! */
				dPlace = 10.0;
				continue;
			}
			else
			{
			/* Put back the errant character. */
				if (toupper(nChar) != 'L')
				{
				/* Put back this character for someone else. */
					UngetStreamChar(nChar);
				}
				break;
			}
			if (dPlace == 1.0)
			{
				dValue = dValue*dBase + (double)nChar;
			}
			else
			{
				dValue += ((double)nChar)/dPlace;
				dPlace *= 10.0;
			}
		}
	}

/* We got a number. */

	if (fNegative)
	{
		dValue = -dValue;
	}

/*
// See if there is an operation next.
// Don't get confused by "//".
*/

	if (m_fParseExpressions)
	{
		nChar = NextNonWhite();

		if (strchr("+-*/|", nChar)
				&& ((nChar != '/') || PeekStreamChar() != '/'))
		{
			double dValue2 = ParseExpression(NextNonWhite());

			switch (nChar)
			{
				case '+':
				{
					dValue += dValue2;
					break;
				}
				case '-':
				{
					dValue -= dValue2;
					break;
				}
				case '*':
				{
					dValue *= dValue2;
					break;
				}
				case '/':
				{
					dValue /= dValue2;
					break;
				}
				case '|':
				{
					dValue = (double)((int)dValue | (int)dValue2);
					break;
				}
			}
		}
		else
		{
		/* Put the character back. */
			UngetStreamChar(nChar);
		}
	}

/* Return what we got. */

	return dValue;
}

/*
// Default IsAlpha().
*/

BOOL CParser::IsAlpha(int nChar)
{
	return((nChar == '_') || isalnum(nChar));
}

/*
// Identify the symbol accumulated so far.
*/

PARSE_TOKEN CParser::IdentifyToken(BOOL fAlnum, BOOL fComplete)
{
	if (fAlnum && !fComplete)
	{
		return TOKEN_Unknown;
	}

	PARSE_TOKEN tToken = LookupSymbolToken(m_cbTokenSymbol);
#if 0
	if (tToken == TOKEN_START_COMMENT);
	{
	/* Found the start of a comment. Parse the end by hand. */

		nChar = '\0';

		for (;;)
		{
			if (nChar == cspEOF)
			{
				ThrowParseException(CParseException::unexpected_end_of_file);
			}

			if (nChar != '*')
			{
				nChar = NextStreamChar();
			}

			if (nChar == '*')
			{
				if ((nChar = NextStreamChar()) == '/')
				{
				/* Found the close comment. */
					break;
				}
			}
		}
		break;
	}
	else
#endif

	if (tToken == TOKEN_LineComment)
	{
		SkipLine();

		if (m_tLookahead == TOKEN_EOF)
		{
			tToken = TOKEN_EOF;
		}
	}
	else if (tToken == TOKEN_LineStatus)
	{
		HandleLineStatus();
		tToken = TOKEN_Unknown;
	}

	return tToken;
}

/*
// Read the next token.
*/

PARSE_TOKEN CParser::NextToken(void)
{
	int nChar;

	m_cbTokenSymbol[0] = '\0';

	for (;;)
	{
	/* Skip white space at the front of the next symbol. */

		if ((nChar = NextNonWhite()) == cspEOF)
		{
			return TOKEN_EOF;
		}

		if (strchr("+-0123456789", nChar)
				|| (nChar == '(' && m_fParseExpressions))
		{
		/* Parse a number. */
			m_dTokenNumber = ParseExpression(nChar);
			return TOKEN_Number;
		}
		else
		{
		/* Parse a token. */

			char *pCurCh = m_cbTokenSymbol;

			BOOL fAlnum = (IsAlpha(nChar) || (nChar == '#'));

			for (;;)
			{
				PARSE_TOKEN tToken;

				*pCurCh++ = (char)nChar;
				*pCurCh = '\0';

				tToken = IdentifyToken(fAlnum, FALSE);

				if (tToken == TOKEN_LineComment)
				{
				/* Begin a token again. */
					break;
				}
				else if (tToken != TOKEN_Unknown)
				{
					return tToken;
				}

				if ((nChar = NextStreamChar()) == cspEOF)
				{
				/* Uh, oh. We hit the end. */
					return TOKEN_EOF;
				}

				BOOL fNewAlnum = IsAlpha(nChar);

				if (fAlnum != fNewAlnum)
				{
				/* We have transcendend another symbol. */

					UngetStreamChar(nChar);

					tToken = IdentifyToken(fAlnum, TRUE);
					if (tToken == TOKEN_LineComment)
					{
					/* Start again. */
						break;
					}
					else if (tToken != TOKEN_Unknown)
					{
						return tToken;
					}

				/* If this is an alphanumeric string, add it as a symbol. */

					if (fAlnum || m_fNonAlphaSymbols)
					{
#if 0
						if (symbol_is_defined(token_symbol))
						{
						/* Use the value for that symbol. */
							token_number = symbol_value(token_symbol);
							return(TOKEN_NUMBER);
						}
#endif
						return TOKEN_Symbol;
					}
					else
					{
					/* Non-alphanumeric string. We just don't know. */
						ThrowParseException(CParseException::unknown_token, m_cbTokenSymbol);
					}
				}
			}
		}
	}

/* This is an unknown token. */

	ThrowParseException(CParseException::unknown_token, m_cbTokenSymbol);

/* It technically shouldn't get here! */

	return TOKEN_Unknown;
}

/*
// Match the next token in the stream.
*/

void CParser::Match(PARSE_TOKEN tToken)
{
	if (tToken == TOKEN_Lookahead)
	{
		tToken = m_tLookahead;
	}

	if (m_tLookahead == tToken)
	{
	/* We matched. */
		m_tLookahead = NextToken();
	}
	else
	{
	/* Oops! Syntax error. */

		if (tToken == TOKEN_EOF)
		{
			ThrowParseException(CParseException::syntax_error, m_tLookahead);
		}
		else
		{
			ThrowParseException(CParseException::unexpected_token, tToken, m_tLookahead);
		}
	}
}

/*
// Set the source file name and the current line number.
*/

void CParser::SetNewFile(const char *pSourceName, DWORD dwLineNo)
{
	m_pSourceFile = pSourceName;
	m_dwLineNo = dwLineNo;
}

/*
// Parse some states.
*/

void CParser::Parse(const PARSE_STATE *states, const TOKEN_LOOKUP *token_lookup)
{
	PARSE_INDEX nIndex, nNewIndex;

/*
// Save our information in the class members.
*/

	if (states != NULL)
	{
		m_states = states;
	}

	if (token_lookup != NULL)
	{
		m_token_lookup = token_lookup;
	}

	nIndex = 0;

/* Validate our state. */

	if (m_states == NULL
			|| m_token_lookup == NULL)
	{
		ThrowParseException(CParseException::bad_arguments);
	}

/* Start at the first index. */

	for (;;)
	{
	/* If the parser had an error, stop now. */

		if (m_tLookahead == TOKEN_Init)
		{
			m_tLookahead = NextToken();
		}

/*		printf("state:%d\n", state);*/

		if (m_tLookahead == m_states[nIndex].m_token
					|| m_states[nIndex].m_token == TOKEN_Anything)
		{
			STATE_PROCESSOR processor;

		/* We match. */

			if ((processor = m_states[nIndex].m_success_func) == NULL)
			{
			/* Remove the next token from the stream. */
				Match();
			}
			else
			{
			/*
			// Invoke the handling function.
 			// The token will be removed by this function if desired.
			// An exception may be thrown.
			*/
				(this->*processor)();
			}
			nNewIndex = m_states[nIndex].m_success_index;
		}
		else
		{
		/* We don't match. */
			nNewIndex = m_states[nIndex].m_failure_index;
		}

	/* Figure out what to do based on this new state. */

		if (nNewIndex == PARSE_ACCEPT)
		{
		/* Good parse. */
			return;
		}

		if (nNewIndex == PARSE_REJECT)
		{
		/* Bad parse. */
			if (m_tLookahead == TOKEN_Symbol)
			{
			/* Complain about this symbol. */
				ThrowParseException(CParseException::symbol_undefined, m_cbTokenSymbol);
			}

			Match(m_states[nIndex].m_token);	/* Generate error message. */
		}		

	/* Not a special state. */

		if (nNewIndex < 0)
		{
		/* Construct a relative state. */
			nIndex += nNewIndex - REL;
		}
		else
		{
		/* Use the absolute state. */
			nIndex = nNewIndex;
		}
	}
}

/*
// Skip to the end of this line.
*/

void CParser::SkipLine(void)
{
	for (;;)
	{
		switch (NextStreamChar())
		{
			case cspEOF:
			{
				/* We hit the end. */
				m_tLookahead = TOKEN_EOF;
				return;
			}
			case '\n':
			{
				m_tLookahead = TOKEN_Init;
				return;
			}
			default:
			{
				break;
			}
		}
	}
}

#if 0
/*
// Start a definition.
*/

PRIVATE BOOL
start_definition()
{
	if ((defining = declare_symbol(token_symbol)) == NULL)
	{
		return(FALSE);
	}
	else
	{
		return(match());
	}
}

/*
// End a definition.
*/

PRIVATE BOOL
end_definition()
{
	BOOL ret;

	ret = FALSE;

	if (defining != NULL)
	{
	/* We are indeed defining. Define the symbol now. */
		ret = (define_symbol(defining->symbol, token_number) != NULL);
		defining = NULL;
	}
	return(ret);
}
#endif

/*
// Throw an exception from strings.
*/

void CParser::ThrowParseException(int nCause, const char *pToken1, const char *pToken2) const
{
/* Throw the exception. */

	THROW(new CParseException(nCause, m_dwLineNo, m_pSourceFile, pToken1, pToken2));
}

/*
// Throw an exception from token ids.
*/

void CParser::ThrowParseException(int nCause, PARSE_TOKEN tToken1, PARSE_TOKEN tToken2) const
{
	char cbToken1[MAX_TOKEN_SIZE], cbToken2[MAX_TOKEN_SIZE];
	char *pToken1, *pToken2;

/* Handle token 1. */

	if (tToken1 == TOKEN_Unknown)
	{
		pToken1 = NULL;
	}
	else
	{
		pToken1 = TokenText(tToken1, cbToken1);
	}

/* Handle token 2. */

	if (tToken2 == TOKEN_Unknown)
	{
		pToken2 = NULL;
	}
	else
	{
		pToken2 = TokenText(tToken2, cbToken2);
	}

/* Throw the exception. */

	THROW(new CParseException(nCause, m_dwLineNo, m_pSourceFile, pToken1, pToken2));
}

/*******************/
/* CParseException */
/*******************/

IMPLEMENT_DYNAMIC(CParseException, CException)

/*
// Constructor.
*/

CParseException::CParseException(int nCause, DWORD dwLineNo, const char *pSourceFile, const char *pToken1, const char *pToken2)
{
	m_nCause = nCause;
	m_dwLineNo = dwLineNo;
/* These may throw exceptions! */
	m_csSourceFile = pSourceFile;
	m_csToken1 = pToken1;
	m_csToken2 = pToken2;
}
