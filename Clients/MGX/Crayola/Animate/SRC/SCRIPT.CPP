/* 
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#include "app.h"
#include "animate.h"
#include "script.h"
#include "object.h"

#ifndef _fstrnicmp
#include <string.h>
#endif

#define I_EOF 	-1

#define ERR_INSUF		1
#define ERR_UNKNOWN		2
#define ERR_INVALID		3

#define STR_INSUF	"Not enough parameters for : "
#define STR_UNKNOWN	"Unknown command."
#define STR_INVALID	"Invalid Animation object."


int latoi(char far *p);


// list of all registered token that can be executed in a script.
// run through the list looking for a token that is encountered
// in a script.  When it is found, get the ID and execute it


//-----CParseToken---------------------------------------------------------------


CParseToken TokenList[] =
{
	CParseToken("MOVE",            I_MOVE, 2),	
	CParseToken("JUMP",            I_JUMP, 2),
	CParseToken("CHILDJUMP",       I_JUMPCHILD, 3),
	CParseToken("PAUSE",           I_PAUSE, 1),
	CParseToken("HIDE",            I_HIDE, 0),	
	CParseToken("SHOW",            I_SHOW, 0),	
	CParseToken("KILL",            I_KILL, 0),	
	CParseToken("BEGIN",           I_BEGIN, 0),	
	CParseToken("REPEAT",          I_REPEAT, 0),	
	CParseToken("SETSPEED",        I_SETSPEED, 1),	
	CParseToken("SS",              I_SETSPEED, 1),	
	CParseToken("INCSPEED",        I_INCSPEED, 0),	
	CParseToken("DECSPEED",        I_DECSPEED, 0),	
	CParseToken("CYCLESPEED",      I_CYCLESPEED, 1),
	CParseToken("CS",              I_CYCLESPEED, 1),
	CParseToken("CHILDCYCLESPEED", I_CHILDCYCLESPEED, 2),
	CParseToken("CCS",             I_CHILDCYCLESPEED, 2),
	CParseToken("ACTFRAME",        I_ACTFRAME, 2),
	CParseToken("DEACTFRAME",      I_DEACTFRAME, 2),
	CParseToken("AF",              I_ACTFRAME, 2),
	CParseToken("DF",              I_DEACTFRAME, 2),
	CParseToken("ACF",             I_ACTCHILDFRAME, 3),
	CParseToken("DCF",             I_DEACTCHILDFRAME, 3),
	CParseToken("FRAMEORDER",      I_FRAMEORDER, 2),
	CParseToken("CHILDFRAMEORDER", I_CHILDFRAMEORDER, 3),
	CParseToken("FRAMEOFF",        I_FRAMEOFFSET, 3),
	CParseToken("CHILDFRAMEOFF",   I_CHILDFRAMEOFFSET, 4),
	CParseToken("FLIPV",           I_FVERT, 0),	
	CParseToken("FLIPH",           I_FHORZ, 0),	
	CParseToken("SETFRAME",        I_SETFRAME, 1),	
	CParseToken("SF",              I_SETFRAME, 1),	
	CParseToken("SETCHILDFRAME",   I_SETCHILDFRAME, 2),	
	CParseToken("SCF",             I_SETCHILDFRAME, 2),	
	CParseToken("REPEATCYCLE",     I_REPEATCYCLE, 1),	
	CParseToken("RC",              I_REPEATCYCLE, 1),	
	CParseToken("WAITLASTFRAME",   I_WAITLASTFRAME, 0),	
	CParseToken("WLF",             I_WAITLASTFRAME, 0),	
	CParseToken("", 0, 0)	
};				   


CParseToken::CParseToken(		// constructor
char far *psName, 
int  nTokenID,
int  nParameters)		
{
	m_lpToken = psName;
	m_ID      = nTokenID;
	m_nParams = nParameters;
}

int CParseToken::GetTokenLen()		// return the length of this token
{
	return lstrlen(m_lpToken);
}


//------CParser--------------------------------------------------------------

CParser::CParser(		// constructor
char 	far *lpScript,		
char 	far *lpError,
int 	nErrorBytes)
{
	m_lpScript = lpScript;
	m_lpError  = lpError;
	m_nErrorBytes = nErrorBytes;
	if (m_lpError)
		memset((void *)m_lpError, 0, m_nErrorBytes);		// clear the error buffer 
	m_nBytesUsed = 1;	// used 1 for the null character already
	m_LineNum = 1;
}


void CParser::SkipBlanks()			// stop at any non white space
{
	char c;
	while ((c = *(m_lpScript++)) && 
			c == ' '  ||
			c == '\t' ||
			c == ',' )
	  if (c == '\r') m_LineNum++;
	  
	 m_lpScript--;
}



void CParser::SeekNextAlpha()		// stop at NULL or alpha character
{
	char c;
	while ((c = *(m_lpScript++)) &&
			((c < 'A' || c > 'Z') &&
			 (c < 'a' || c > 'z')))
	  if (c == '\r') m_LineNum++;
	  
	 m_lpScript--;
}


void CParser::SeekNextNumeric()		// stop at NULL or 0..9
{
	char c;
	while ((c = *(m_lpScript++)) &&
			(c < '0' || c > '9'))
	  if (c == '\r') m_LineNum++;

	 m_lpScript--;
}



bool CParser::GetNextNum(int &Val)		// get value of next number - must be 0..9
{
	if (!m_lpScript) return 0;
	bool bResult = FALSE;
	SkipBlanks();						// go to anything
	char far *lpNum = m_lpScript;		// start of the number
	char c;
	
	while ((c = *(m_lpScript++)) &&
           ((c >= '0' && c <= '9') || c == '-'))
	  bResult = TRUE;
	  
	m_lpScript--;						// next char after the number
	if (bResult)
		Val = latoi(lpNum);
	return bResult;
}	  



int CParser::GetNextWord()		// get length of next word - must be alpha
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


int CParser::GetNextToken()		// return next token in the script
{
	if (!m_lpScript) return I_EOF;

	int i;
	int wWordLen, wLen;
	CParseToken *pToken;

#ifdef WIN32
	char Word[100];
#else
	char far *pWord;
#endif
	
	if (wWordLen = GetNextWord())
	{
#ifdef WIN32
		CopyMemory(&Word, m_lpScript, wWordLen);
		Word[wWordLen] = 0;
#else
		pWord = m_lpScript;			// the word it just located
#endif
		m_lpScript += wWordLen;		// just after this word
		
		for (i=0; (pToken = &TokenList[i]) && pToken->GetID(); i++)
		{
			char far *lpToken = pToken->GetToken();
			wLen = pToken->GetTokenLen(); 
			if ((wWordLen == wLen) &&
#ifdef WIN32
                (lstrcmpi(lpToken, (LPSTR)&Word) == 0))
#else
                (_fstrnicmp(lpToken, pWord, wLen) == 0))
#endif
			{
				bool bResult = TRUE;
				for (int q=0; bResult && q<pToken->GetParams(); q++)
					bResult = GetNextNum(m_Params[q]);
				if (!bResult)
					RecordError(pToken, ERR_INSUF);
				return bResult ? pToken->GetID() : I_ERROR;	// found one.
			}	
		}
		// cannot find that command		
		char command[100];
		command[0] = 0;
#ifdef WIN32		
		CopyMemory(command, &Word, wWordLen+1);
#else
		lstrcpyn(command, pWord, wWordLen+1);
#endif
		CParseToken Dummy(command, 0, 0);
		RecordError(&Dummy, ERR_UNKNOWN);
		return I_ERROR;
	}
	// no more words in the script
	return I_EOF;
}


int latoi(char far *p)
{
	int len = 0, i = 0;
	int mult = 1;
	int result = 0;
	int ch;
    bool bNeg = FALSE;
    if (*p == '-')
    {
        bNeg = TRUE;
        p++;
    }
	
	char far *p1 = p;
	while (((ch = p1[0]) >= '0') && ch <= '9')
	{
		len++;
		p1++;
		mult *= 10;
	}
	
	for (i=0; i<len; i++)
	{	
		mult /= 10;		
		result += ((p[i] - '0') * mult);
	}
	
	return (bNeg ? -result : result); 
}





void CParser::RecordError(			// record in the error buffer
CParseToken *pToken, 
int error)	
{
	if (!m_lpError) return;
	char sError[100];				// temporary buffer to put this message into
	char sLineNum[20];
	wsprintf(sLineNum, "Line: %i ", m_LineNum);
	char far *lpError = &sError[0];
	int len = 0;
	
	switch (error)					// which error is it
	{
		case ERR_INSUF:
			len = wsprintf(sError, "%s%s\"%s\"\r\n", 
					(char far *)sLineNum, 
					(char far *)STR_INSUF, 
					pToken->GetToken());
			break;
		case ERR_UNKNOWN:
			len = wsprintf(sError, "%s \"%s\" %s\r\n", 
					(char far *)sLineNum, 
					pToken->GetToken(), 
					(char far *)STR_UNKNOWN);
			break;
		case ERR_INVALID:
			len = wsprintf(sError, "%s\r\n", 
					(char far *)STR_INVALID);
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





//-------CAnimator-------------------------------------------------------------


void CAnimator::SetCurrentScript(int dAobject, char far *lpScript)
{
// Keep a copy of the script.  Now you can re-execute it later.

// Find the object that this script belongs to
	CAnimateObj* thisOne = FindAobject(dAobject);
    if (!thisOne) return;

// Free the old scrip if there is one.
    HGLOBAL hScript = thisOne->m_hScript;
    if (hScript)
    {
        GlobalFree(hScript);
        thisOne->m_hScript = NULL;
    }

// Set in the new script
    hScript = GlobalAlloc(GHND, lstrlen(lpScript) + 1);
    if (hScript)
    {
        LPSTR lpGlobal = (char far *)GlobalLock(hScript);
        if (lpGlobal)
        {
            lstrcpy(lpGlobal, lpScript); 
            GlobalUnlock(hScript);
            thisOne->m_hScript = hScript;
        }
    }
}



bool CAnimator::ExecuteScriptSetup(
int 	dAobject, 
char 	far *lpScript,
char 	far *lpError,
int 	nErrorBytes)
{
// This function sets up the script for the object as well as executes it.
	CParser Parser(lpScript, lpError, nErrorBytes);
	CAnimateObj* thisOne = FindAobject(dAobject);
	if (!thisOne)
	{
		Parser.RecordError(NULL, ERR_INVALID);
		return FALSE;
	}

    SetCurrentScript(dAobject, lpScript);
    return ExecuteScript(dAobject, lpScript, lpError, nErrorBytes);
}






bool CAnimator::ExecuteScript(
int 	dAobject, 
char 	far *lpScript,
char 	far *lpError,
int 	nErrorBytes)
{
	int x, y;
	bool bResult;
	CParser Parser(lpScript, lpError, nErrorBytes);
	bool bValidObject = TRUE;
	if (!FindAobject(dAobject))
	{
		Parser.RecordError(NULL, ERR_INVALID);
		bValidObject = FALSE;
	}
	                        
	int instr;
    int i;
	while ((instr = Parser.GetNextToken()) && bValidObject)
	{
		x = Parser.GetParam(0);
		y = Parser.GetParam(1);
		
		switch (instr)		// token from the script file
		{
			case I_MOVE: 		    Move(dAobject, x, y); 		                break;
			case I_JUMP: 		    Jump(dAobject, x, y); 		                break;
			case I_HIDE: 		    Hide(dAobject); 			                break;
			case I_SHOW: 		    Show(dAobject); 			                break;
			case I_SPEED: 		    SetSpeed(dAobject, x); 		                break;
			case I_PAUSE: 		    Pause(dAobject, x); 		                break;
			case I_WAITLASTFRAME: 	WaitLastFrame(dAobject); 		            break;
			case I_BEGIN: 		    BeginCycle(dAobject); 		                break;
			case I_REPEAT: 		    RepeatCycle(dAobject); 		                break;
			case I_TRACK: 		    StartTrack(dAobject); 		                break;
			case I_STOPTRACK: 	    StopTrack(dAobject); 		                break;
			case I_KILL: 		    Kill(dAobject); 			                break;
			case I_FVERT: 		    FlipVertical(dAobject); 	                break;
			case I_FHORZ: 		    FlipHorizontal(dAobject); 	                break;
			case I_SETLIMIT: 	    SetTrackLimit(dAobject, x);                 break;
			case I_SETSPEED: 	    SetSpeed(dAobject, x); 		                break;
			case I_INCSPEED: 	    IncSpeed(dAobject); 		                break;
			case I_DECSPEED: 	    DecSpeed(dAobject); 		                break;
            case I_CYCLESPEED:      SetFramesPerSec(dAobject, x);               break;
            case I_CHILDCYCLESPEED: SetChildFramesPerSec(dAobject, x, y);       break;
            case I_ACTFRAME:        ActivateFrame(dAobject, x, y, TRUE);        break;
            case I_DEACTFRAME:      ActivateFrame(dAobject, x, y, FALSE);       break;
            case I_FRAMEORDER:      SetFrameOrder(dAobject, x, y);              break;
            case I_SETFRAME:        SetCurrentFrame(dAobject, x);               break;
            case I_SETCHILDFRAME:   SetCurrentChildFrame(dAobject, x, y);       break;
            case I_REPEATCYCLE:     SetRepeatCycle(dAobject, x);                break;
            case I_CHILDFRAMEORDER: SetChildFrameOrder(dAobject, x, y, 
                                        Parser.GetParam(2));      
                break;
            case I_JUMPCHILD:
                i = GetChildObjectID(dAobject, Parser.GetParam(2));       
                Jump(i, x, y); 
                break;
            case I_ACTCHILDFRAME:   
                                    ActivateChildFrame(dAobject, 
                                        Parser.GetParam(0), 
                                        Parser.GetParam(1),
                                        Parser.GetParam(2), 
                                        TRUE);  
                break;
            case I_DEACTCHILDFRAME: 
                                    ActivateChildFrame(dAobject, 
                                        Parser.GetParam(0), 
                                        Parser.GetParam(1),
                                        Parser.GetParam(2), 
                                        FALSE);  
                break;
            case I_FRAMEOFFSET:     SetFrameOffset(dAobject, 
                                        Parser.GetParam(0), 
                                        Parser.GetParam(1), 
                                        Parser.GetParam(2));             
                break;
            case I_CHILDFRAMEOFFSET: SetChildFrameOffset(dAobject, 
                                        Parser.GetParam(0), 
                                        Parser.GetParam(1), 
                                        Parser.GetParam(2),             
                                        Parser.GetParam(3));             
                break;
			
			case I_EOF:
                #ifdef _DEBUG 
				OutputDebugString(lpError);
                #endif
				return TRUE;
		}
	}
#ifdef _DEBUG
	OutputDebugString(lpError);
#endif 	
	
	return bValidObject;
}



void CAnimator::ReExecuteScript(int dAobject)
{
// Re-execute the last script that was given to the object
	CAnimateObj *thisOne = FindAobject(dAobject);
	if (!thisOne) return;

    HGLOBAL hScript = thisOne->m_hScript;
    if (hScript)
    {
        LPSTR lpGlobal = (char far *)GlobalLock(hScript);
        if (lpGlobal)
        {
            thisOne->DeleteAllInstr();
            ExecuteScript(dAobject, lpGlobal, NULL, 0);
            GlobalUnlock(hScript);
        }
    }

// Begin executing all the child scripts.
    CAnimateObj *pChild = thisOne->m_ChildHead;
    while (pChild)
    {
        ReExecuteScript(pChild->descriptor);
        pChild = pChild->next;
    }
}
