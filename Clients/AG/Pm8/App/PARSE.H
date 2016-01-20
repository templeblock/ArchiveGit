/*
// $Header: /PM8/App/PARSE.H 1     3/03/99 6:08p Gbeddow $
//
// Stream Parser definitions.
//
// $Log: /PM8/App/PARSE.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:29:30   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:18   FRED
// Initial revision.
// 
//    Rev 1.0   27 Apr 1995 13:30:12   FRED
// Initial revision.
// 
//    Rev 1.0   04 Nov 1994 16:00:20   JAY
// Initial revision.
*/

#ifndef __PARSE_H__
#define __PARSE_H__

/*
// The classes defined here.
*/

class CParseException;
class CParser;				/* Stream parser. */

/*
// Parse indices.
//
// Positive values are indices back into the state table.
// ACCEPT (-1) and REJECT (-2) are special indices.
// Other negative values are relative indices based on REL.
*/

typedef	int PARSE_INDEX;

#define	PARSE_ACCEPT	((PARSE_INDEX)(-1))		/* We have parsed successfully. */
#define	PARSE_REJECT	((PARSE_INDEX)(-2))		/* We have failed our parse. */

#define	REL		((PARSE_INDEX)(-16384))		/* A relative value. */

/*
// A parse exception.
*/

class CParseException : public CException
{
	DECLARE_DYNAMIC(CParseException);
public:
/* Cause values. */
	enum
	{
		none = 0,
		unknown = 1,
		bad_arguments = 2,
		internal_error = 3,
		unexpected_token = 4,
		unknown_token = 5,
		syntax_error = 6,
		symbol_undefined = 7,
		symbol_redefined = 8,
		number_expected = 9,
		illegally_formed_line_status = 10,			/* Bad '#line'. */
		unexpected_end_of_file = 11,
		bad_expression = 12,
	/*
	// You can add your own!
	*/
		USER_DEFINED = 0x2000
	};

/* Constructor */

	CParseException(int nCause, DWORD dwLineNo, const char *pSourceFile, const char *pToken1 = NULL, const char *pToken2 = NULL);

/* Attributes */

	int m_nCause;
	DWORD m_dwLineNo;
	CString m_csSourceFile;
	CString m_csToken1;
	CString m_csToken2;
};

/*
// Parse tokens.
//
// Negative token values are internally defined.
*/

typedef	int PARSE_TOKEN;

/* Internal tokens used by the parser. */

enum INTERNAL_TOKEN
{
	TOKEN_Lookahead = (int)0x8000,		/* Match 'lookahead'. */
	TOKEN_EndOfList,							/* End of token list. */
	TOKEN_Init,									/* Token needs to be initialized. */
	TOKEN_Unknown,								/* An unknown token. */
	TOKEN_EOF,									/* The end of file. */
	TOKEN_Number,								/* A number token. */
	TOKEN_Symbol,								/* A symbol. */
	TOKEN_LineStatus,							/* A line status line. */
	TOKEN_LineComment,						/* A line comment. */
	TOKEN_Anything,							/* Match anything. */
};

/* The maximum length of a token name. */

#define	MAX_TOKEN_SIZE	128

/*
// A token lookup.
//
// A pairing between a token and the symbol it represents.
// These usually come in arrays.
*/

struct TOKEN_LOOKUP
{
	PARSE_TOKEN	m_tToken;		/* The token number corresponding to the text. */
	const char	*m_pSymbol;		/* The string for this token. */
};

/*
// A stream parser.
*/

class CParser
{
private:
/*
// These MUST be overridden in a derived class, and they should ONLY be
// called by the base class (to maintain the accuracy of m_dwLineNo).
*/

	virtual int NextChar(void) = 0;
	virtual void UngetChar(int) = 0;

public:
/*
// A state processor.
// This is a method of this class which is placed in a
// PARSE_STATE and called upon match of the token in that state.
//
// This is allowed to throw a CParseException.
*/

	typedef void (CParser::* STATE_PROCESSOR)(void);

/*
// A parse state.
//
// This state points to other states depending on success or failure in
// matching a token.
*/

	struct PARSE_STATE
	{
		PARSE_TOKEN			m_token;				/* What token to expect. */
		STATE_PROCESSOR	m_success_func;	/* An optional function to call on success. */
		PARSE_INDEX			m_success_index;	/* What state to go to if we match. */
		PARSE_INDEX			m_failure_index;	/* What state to go to if we fail. */
	};

protected:

/*
// Data.
*/

	BOOL m_fIgnoreCase;
	BOOL m_fParseExpressions;
	BOOL m_fNonAlphaSymbols;

	DWORD m_dwLineNo;
	PARSE_TOKEN m_tLookahead;
	const char *m_pSourceFile;

	double m_dTokenNumber;
	char m_cbTokenSymbol[MAX_TOKEN_SIZE];	/* The current token's symbol. */

/* The specific states for this parse. */

	const PARSE_STATE *m_states;
	const TOKEN_LOOKUP *m_token_lookup;

/*
// Implementation.
*/

/* Interface to the stream object which manages the line number. */

	enum
	{
		cspEOF = -1
	};

	virtual int NextStreamChar(void);
	virtual int PeekStreamChar(void);
	virtual void UngetStreamChar(int);

	int NextNonWhite(void);
	void HandleLineStatus(void);

/* Get the next token. */

	virtual PARSE_TOKEN NextToken(void);

/* Return the symbol for a token. This includes all the internal tokens. */

	virtual char *TokenText(PARSE_TOKEN tToken, char *buffer) const;

/*
// Identify the symbol accumulated so far.
*/

	virtual PARSE_TOKEN IdentifyToken(BOOL fAlnum, BOOL fComplete);

/*
// Lookup a token given a symbol.
// NULL for token_lookup means search all lookups (watch for recursion).
*/

	virtual PARSE_TOKEN LookupSymbolToken(const char *pSymbol, const TOKEN_LOOKUP *token_lookup = NULL) const;

/*
// Lookup a symbol given a token.
// NULL for token_lookup means search all lookups (watch for recursion).
*/

	virtual const char *LookupTokenSymbol(PARSE_TOKEN tToken, const TOKEN_LOOKUP *token_lookup = NULL) const;

/*
// Reset the parser.
*/

	virtual VOID Init(VOID)
		{ m_dwLineNo = 1L; m_tLookahead = TOKEN_Init; }

/*
// Parse an expression.
*/

	virtual double CParser::ParseExpression(int nChar);

/*
// Is a character an alphanumeric?
*/

	BOOL IsAlpha(int nChar);

public:
/*
// Construct a CParser.
*/

	CParser(BOOL fIgnoreCase = TRUE, BOOL fParseExpressions = TRUE, BOOL fNonAlphaSymbols = FALSE);

/*
// Virtual destructor to enable derived classes.
*/

	virtual ~CParser() {}

/*
// Parse a file.
//
// Default arguments allow middle-of-file transitions, though they all need to
// be defined for the initial parse.
// May generate a CParseException.
*/

	virtual void Parse(const PARSE_STATE *states, const TOKEN_LOOKUP *token_lookup = NULL);

/*
// Match the next token in the stream.
//
// Generates a CParseException if the tokens do not match.
// Default argument matches anything.
*/

	virtual void Match(PARSE_TOKEN token = TOKEN_Lookahead);

/*
// Set the source file name and the current line number.
*/

	virtual void SetNewFile(const char *pSourceName, DWORD dwLineNo = 1L);

/*
// Skip the line we are currntly on.
*/

	virtual void SkipLine(void);

/*
// Throw a parse exception.
*/

	void ThrowParseException(int nCause, const char *pToken1 = NULL, const char *pToken2 = NULL) const;
	void ThrowParseException(int nCause, PARSE_TOKEN tToken1, PARSE_TOKEN tToken2 = TOKEN_Unknown) const;
};

#endif					// #ifndef __PARSE_H__
