#include "windows.h"
#include "proto.h"
#include "sprite.h"
#include "script.h"

#ifndef _fstrnicmp
#include <string.h>
#endif

#define I_EOF			-1
#define ERR_INSUF		1
#define ERR_UNKNOWN		2
#define ERR_INVALID		3

#define STR_INSUF	"Not enough parameters for : "
#define STR_UNKNOWN "Unknown command."
#define STR_INVALID "Invalid Animation sprite."

// list of all registered token that can be executed in a script.
// run through the list looking for a token that is encountered
// in a script.	 When it is found, get the ID and execute it

//-----CParseToken---------------------------------------------------------------

CParseToken TokenList[] =
{
	CParseToken("MOVE",			I_MOVE, 2),
	CParseToken("JUMP",			I_JUMP, 2),
	CParseToken("PAUSE",		I_PAUSE, 1),
	CParseToken("HIDE",			I_HIDE, 0),
	CParseToken("SHOW",			I_SHOW, 0),
	CParseToken("KILL",			I_KILL, 0),
	CParseToken("BEGIN",		I_BEGIN, 0),
	CParseToken("REPEAT",		I_REPEAT, 0),
	CParseToken("SETSPEED",		I_SETSPEED, 1),
	CParseToken("SS",			I_SETSPEED, 1),
	CParseToken("INCSPEED",		I_INCSPEED, 0),
	CParseToken("DECSPEED",		I_DECSPEED, 0),
	CParseToken("CELLSPERSEC",	I_SETCELLSPERSEC, 1),
	CParseToken("CPS",			I_SETCELLSPERSEC, 1),
	CParseToken("ACTCELL",		I_ACTCELL, 2),
	CParseToken("AF",			I_ACTCELL, 2),
	CParseToken("DEACTCELL",	I_DEACTCELL, 2),
	CParseToken("DF",			I_DEACTCELL, 2),
	CParseToken("REVERSE",		I_REVERSE_CELLORDER, 0),
	CParseToken("CELLORDER",	I_CELLORDER, 2),
	CParseToken("FLIPV",		I_FVERT, 0),
	CParseToken("FLIPH",		I_FHORZ, 0),
	CParseToken("SETCELL",		I_SETCELL, 1),
	CParseToken("SF",			I_SETCELL, 1),
	CParseToken("REPEATCYCLE",	I_REPEATCYCLE, 1),
	CParseToken("RC",			I_REPEATCYCLE, 1),
	CParseToken("PAUSEAFTERCELL",I_PAUSEAFTERCELL, 1),
	CParseToken("PAC",			I_PAUSEAFTERCELL, 1),
	CParseToken("", 0, 0)
};

// constructor
//***********************************************************************
CParseToken::CParseToken( LPSTR psName, int nTokenID, int nParameters )
//***********************************************************************
{
	m_lpToken = psName;
	m_ID	= nTokenID;
	m_nParams = nParameters;
}

//***********************************************************************
int CParseToken::GetTokenLen()		// return the length of this token
//***********************************************************************
{
	return lstrlen(m_lpToken);
}

//------CParser--------------------------------------------------------------

// constructor
//***********************************************************************
CParser::CParser( LPSTR lpScript, LPSTR lpError, int nErrorBytes )
//***********************************************************************
{
	m_lpScript = lpScript;
	m_lpError  = lpError;
	m_nErrorBytes = nErrorBytes;
	if (m_lpError)
		_fmemset(m_lpError, 0, m_nErrorBytes);		// clear the error buffer
	m_nBytesUsed = 1;	// used 1 for the null character already
	m_LineNum = 1;
}

//***********************************************************************
void CParser::SkipBlanks()			// stop at any non white space
//***********************************************************************
{
	char c;
	while ((c = *(m_lpScript++)) &&
			c == ' '  ||
			c == '\t' ||
			c == ',' )
	if (c == '\r') m_LineNum++;

	m_lpScript--;
}

//***********************************************************************
void CParser::SeekNextAlpha()		// stop at NULL or alpha character
//***********************************************************************
{
	char c;
	while ((c = *(m_lpScript++)) &&
			((c < 'A' || c > 'Z') &&
			(c < 'a' || c > 'z')))
	if (c == '\r') m_LineNum++;

	m_lpScript--;
}

//***********************************************************************
void CParser::SeekNextNumeric()		// stop at NULL or 0..9
//***********************************************************************
{
	char c;
	while ((c = *(m_lpScript++)) &&
			(c < '0' || c > '9'))
	if (c == '\r') m_LineNum++;

	m_lpScript--;
}

//***********************************************************************
BOOL CParser::GetNextNum(LPINT Val)		// get value of next number - must be 0..9
//***********************************************************************
{
	if (!m_lpScript) return 0;
	BOOL bResult = FALSE;
	SkipBlanks();						// go to anything
	LPSTR lpNum = m_lpScript;		// start of the number
	char c;

	while ((c = *(m_lpScript++)) &&
			((c >= '0' && c <= '9') || c == '-'))
	bResult = TRUE;

	m_lpScript--;						// next char after the number
	if (bResult)
		*Val = latoi(lpNum);
	return bResult;
}

//***********************************************************************
int CParser::GetNextWord()		// get length of next word - must be alpha
//***********************************************************************
{
	if (!m_lpScript) return 0;
	SeekNextAlpha();
	int i = 0;
	char c;
	while ((c = *(m_lpScript++)) &&
			((c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c >= 'z')))
	i++;
	m_lpScript -= i+1;
	return i;
}

//***********************************************************************
int CParser::GetNextToken()		// return next token in the script
//***********************************************************************
{
	if (!m_lpScript) return I_EOF;

	int i;
	int wWordLen, wLen;
	pCParseToken pToken;

	LPSTR pWord;

	if (wWordLen = GetNextWord())
	{
		pWord = m_lpScript;			// the word it just located
		m_lpScript += wWordLen;		// just after this word

		for (i=0; (pToken = &TokenList[i]) && pToken->GetID(); i++)
		{
			LPSTR lpToken = pToken->GetToken();
			wLen = pToken->GetTokenLen();
			if ((wWordLen == wLen) &&
				(_fstrnicmp(lpToken, pWord, wLen) == 0))
			{
				BOOL bResult = TRUE;
				for (int q=0; bResult && q<pToken->GetParams(); q++)
					bResult = GetNextNum(&m_Params[q]);
				if (!bResult)
					RecordError(pToken, ERR_INSUF);
				return bResult ? pToken->GetID() : NULL;	// found one.
			}
		}
		// cannot find that command
		char command[100];
		command[0] = 0;
		lstrcpyn(command, pWord, wWordLen+1);
		CParseToken Dummy((LPSTR)command, 0, 0);
		RecordError(&Dummy, ERR_UNKNOWN);
		return NULL;
	}
	// no more words in the script
	return I_EOF;
}

//***********************************************************************
void CParser::RecordError(			// record in the error buffer
pCParseToken pToken,
int error)
//***********************************************************************
{
	if (!m_lpError) return;
	char sError[100];				// temporary buffer to put this message into
	char sLineNum[20];
	wsprintf(sLineNum, "Line: %i ", m_LineNum);
	LPSTR lpError = &sError[0];
	int len = 0;

	switch (error)					// which error is it
	{
		case ERR_INSUF:
			len = wsprintf(sError, "%s%s\"%s\"\r\n",
					(LPSTR )sLineNum,
					(LPSTR )STR_INSUF,
					pToken->GetToken());
			break;
		case ERR_UNKNOWN:
			len = wsprintf(sError, "%s \"%s\" %s\r\n",
					(LPSTR )sLineNum,
					pToken->GetToken(),
					(LPSTR )STR_UNKNOWN);
			break;
		case ERR_INVALID:
			len = wsprintf(sError, "%s\r\n",
					(LPSTR )STR_INVALID);
			break;
	}

	int nBytesLeft = m_nErrorBytes - m_nBytesUsed;	// don't overrun the buffer
	len = min(len, nBytesLeft);
	if (len > 0)
	{
		while (len--)
			*m_lpError++ = *lpError++;		// copy the error message to buffer
		m_nBytesUsed += len;				// update the bytes used so far
	}
}
