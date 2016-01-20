/* 
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

class CParseToken;


//-----------------------------CParser------------------------------------

#define MAX_PARAMS		10

// CParser is the class that does all the dirty work of parsing the 
// script.  It scans for words, then tries to find those words in
// the token list.  If they are in thelist, then it tries to get
// enough tokens for them.  If all is successful then the actual
// call to the animator is made to send an instruction to an object.
// if an error occurs while scaning for a token then an error
// message is placed in the error buffer.  The applicaton can do
// whatever it wants with the mesage.
class CParser
{
public:
	CParser(
		char 	far *lpScript,		
		char 	far *lpError,
		int 	nErrorBytes);
	int GetNextToken();
	int GetParam(int val) { return m_Params[val]; };
	void RecordError(				// record in the error buffer
		CParseToken *pToken, 
		int error);	

private:
	bool GetNextNum(int &Val);		// get value of next number - must be 0..9
	int  GetNextWord();
	void SkipBlanks();
	void SeekNextAlpha();			// go to the next alpha character
	void SeekNextNumeric();			// stop at NULL or 0..9
	
	char far *m_lpScript;			// current location in the script
	char far *m_lpError;			// buffer to put error messages into
	int  m_nErrorBytes;				// number of bytes the error buffer is
	int  m_nBytesUsed;				// how many bytes used in the error message
	int  m_Params[MAX_PARAMS];		// parameters for script commands
	int  m_LineNum;					// which line it is currently on
};





//-----------------------------CParseToken------------------------------------

// CParseToken's are small objects that hold basic token information,
// such as the token text, the ID and the number of parameters that
// instruction takes.  There is a list of these that the parser looks
// through to find a match for each word in the script.  When a match
// is found, the ID can be used to send a message to an animation object.
class CParseToken
{
public:
	CParseToken(		// constructor
	char far *psName, 
	int  nTokenID,
	int  nParameters);
			
	char far *GetToken() { return m_lpToken; };
	int GetTokenLen();
	int GetID() { return m_ID; };
	int GetParams() { return m_nParams; };
	
private:
	char far *m_lpToken;		// text
	int  m_ID;					// instruction token
	int  m_nParams;
};


