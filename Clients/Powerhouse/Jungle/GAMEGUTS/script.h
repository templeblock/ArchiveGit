#ifndef _SCRIPT_H_
#define _SCRIPT_H_

// CParseToken's are small sprites that hold basic token information,
// such as the token text, the ID and the number of parameters that
// action takes.  There is a list of these that the parser looks
// through to find a match for each word in the script.	 When a match
// is found, the ID can be used to send a message to an animation sprite.

typedef class CParseToken far * pCParseToken;

class far CParseToken
{
public:
	CParseToken( LPSTR psName, int nTokenID,	int nParameters );
	LPSTR GetToken()		{ return m_lpToken; };
	int GetTokenLen();
	int GetID()			{ return m_ID; };
	int GetParams()		{ return m_nParams; };
private:
	LPSTR m_lpToken;		// text
	int m_ID;			// action token
	int m_nParams;
};

//-----------------------------CParser------------------------------------

#define MAX_PARAMS		10

// CParser is the class that does all the dirty work of parsing the 
// script.	It scans for words, then tries to find those words in
// the token list.	If they are in thelist, then it tries to get
// enough tokens for them.	If all is successful then the actual
// call to the animator is made to send an action to an sprite.
// if an error occurs while scaning for a token then an error
// message is placed in the error buffer.  The applicaton can do
// whatever it wants with the mesage.

typedef class CParser far * pCParser;

class far CParser
{
public:
	CParser( LPSTR lpScript, LPSTR lpError, int nErrorBytes );
	int GetNextToken();
	int GetParam(int val) { return m_Params[val]; };
	void RecordError( pCParseToken pToken, int error );
private:
	BOOL GetNextNum(LPINT Val);		// get value of next number - must be 0..9
	int GetNextWord();
	void SkipBlanks();
	void SeekNextAlpha();			// go to the next alpha character
	void SeekNextNumeric();			// stop at NULL or 0..9
	LPSTR m_lpScript;				// current location in the script
	LPSTR m_lpError;					// buffer to put error messages into
	int m_nErrorBytes;				// number of bytes the error buffer is
	int m_nBytesUsed;				// how many bytes used in the error message
	int m_Params[MAX_PARAMS];		// parameters for script commands
	int m_LineNum;					// which line it is currently on
};

#endif _SCRIPT_H_
