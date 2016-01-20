////////////////////////////////////////////////////////////////////////////////
//	document.h															      //
//																			  //
//	The code for dealing with documents										  //
//	James Vogh																  //
//  June 4, 1996															  //
////////////////////////////////////////////////////////////////////////////////

#ifndef IN_DOCUMENT
#define IN_DOCUMENT

#ifdef IN_MAC_OS_CODE

#include "mcotypes.h"
#include "mcostat.h"

#include "basewin.h"



class Document {
private:
protected:

	baseWin **wins;
	int32	numWins;
	int32	currentMax;	
	
public:

	int		locked;

	Boolean needsSave;

	Document(void);
	virtual ~Document();
	// if memory is not allocated then this is false
	Boolean validData;
	// Add a window to the list of windows
	// The first window should always be the main window
	McoStatus addWindow(baseWin *win);
	// Remove a window from the list of windows
	virtual McoStatus removeWindow(baseWin *win);
	// see if the window belongs to the documnet
	Boolean isMyWindow(baseWin *win);
	// check to see if the window is the main window of the documnet
	// if so, then closing that window should also close the document
	Boolean isMyMainWindow(baseWin *win);
	// return the main window
	baseWin *getMainWIndow(void);
	// find a window to close, this function is called when the documnet is being closed
	baseWin *findClosableWin(void);
	// get the window with the specified type and number
	baseWin *getWindow(MS_WindowTypes *winType,int32 *winNum);
	virtual Boolean dataPresent(void) {return FALSE;}
	virtual McoStatus doCommand(WindowCode *wc,WindowCode *wc2,void **data,Boolean *changed);

	virtual Boolean checkCloseDoc(void) {return TRUE;}
	
	virtual int	validFile(void) { return 0;}
	
	virtual int	checkDocSave(void) { return 0;}
	virtual int	checkDocCanBeSave(void) { return 0;}
	virtual int isMeasuring(void) { return 0;}
	
};

#else

#include "mcotypes.h"
#include "mcostat.h"


class Document {
private:
protected:


	
public:

	int		locked;

	Boolean needsSave;
	
	// if memory is not allocated then this is false
	Boolean validData;

	Document(void) {validData = 0; locked = 0; needsSave = 0;}
	virtual ~Document() {}

	virtual Boolean dataPresent(void) {return FALSE;}

	virtual Boolean checkCloseDoc(void) {return TRUE;}
	
	virtual int	validFile(void) { return 0;}
	
	virtual int	checkDocSave(void) { return 0;}
	virtual int	checkDocCanBeSave(void) { return 0;}
	virtual int isMeasuring(void) { return 0;}
	
};
#endif

#endif