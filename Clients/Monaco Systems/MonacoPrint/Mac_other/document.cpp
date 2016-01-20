////////////////////////////////////////////////////////////////////////////////
//	document.c															      //
//																			  //
//	The code for dealing with documents										  //
//	James Vogh																  //
//  June 4, 1996															  //
////////////////////////////////////////////////////////////////////////////////

#include "document.h"

Document::Document(void)
{
wins = new baseWin*[MaxNumWins];
numWins = 0;
validData = TRUE;
needsSave = FALSE;
locked = 0;
}

Document::~Document(void)
{
// windows do not actually belong to the documnet, 
// the main event handler owns all windows
delete wins;
}

// Add a window to the list of windows
// The first window should always be the main window
McoStatus Document::addWindow(baseWin *win)
{
int i;

for (i=0; i<numWins; i++) if (win == wins[i]) return MCO_SUCCESS;
if (i >= MaxNumWins) return MCO_FAILURE;
wins[i] = win;
numWins++;
return MCO_SUCCESS;
}

// Remove a window from the list of windows
McoStatus Document::removeWindow(baseWin *win)
{
int i,j;

for (i=0; i<numWins; i++) if (win == wins[i]) break;
if (i == numWins) return MCO_SUCCESS;
for (j=i; j<numWins; j++)
	{
	wins[j] = wins[j+1];
	}
numWins--;
return MCO_SUCCESS;
}

// see if the window belongs to the documnet
Boolean Document::isMyWindow(baseWin *win)
{
int i;

for (i=0; i<numWins; i++) if (win == wins[i]) return TRUE;
return FALSE;
}

// check to see if the window is the main window of the documnet
// if so, then closing that window should also close the document
Boolean Document::isMyMainWindow(baseWin *win)
{
int i;

if (numWins == 0) return FALSE;
if (wins[0] == win) return TRUE;
return FALSE;
}

baseWin *Document::getMainWIndow(void)
{
return wins[0];
}

// find a window to close, this function is called when the documnet is closed
baseWin *Document::findClosableWin(void)
{
if (numWins > 0) return wins[numWins-1];
return 0L;
}

// get the window with the specified type and number and return number if specifed number is -1
baseWin *Document::getWindow(MS_WindowTypes *winType,int32 *winNum)
{
int i;

for (i=0; i<numWins; i++) if ((wins[i]->isMyWindowType(*winType)) && (wins[i]->isMyWindowNum(*winNum))) 
	{
	*winNum = wins[i]->WinNum; // could have been -1
	return wins[i];
	}
return 0L;
}

// Do a command, since documnet is an empty object, do nothing here
McoStatus Document::doCommand(WindowCode *wc,WindowCode *wc2,void **data,Boolean *changed)
{

return MCO_UNSUPPORTED_OPERATION;
}





