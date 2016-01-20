/*      thermwin.h
		
		Draw a dialog with a thermometer
		
		Copyright 1993-97, Monaco Systems Inc.
		by James Vogh
		
*/



#ifndef IN_THERMWIN
#define IN_THERMWIN

#include <Threads.h>
#include "mcostat.h"
#include "process_states.h"
#include "basewin.h"
#include "thermobject.h"
#include "gamutwindata.h"
#include "document.h"

typedef enum {
	THWIN_DLG = 0,
	THWIN_BOX,
	THWIN_TEXT,
	THWIN_CANCEL,
	THWIN_ALL
	} THWIN_IDS;
	
#define NUM_THWIN_IDS 5

#define PROCESS_STATE_IDS { 1,3,4,5,6}


/* The data structure for the thermometer dialog */

class ThermWin:public baseWin {
private:
protected:

Document *doc;

ThermObject	*thermd;

short		ids[NUM_THWIN_IDS+4];
long		max_k;
long		last_k;
long		current_k;

ProcessingStates pstate,lastpstate;

int			finished;
int			quit;

ThreadID	threadID;
long		threadResult;

long		_last_tick_time2;

GammutWinData *gdata;

public:
McoStatus 	error;


ThermWin(Document *dc,ProcessingStates ps,long max,MS_WindowTypes wint,int winn);
~ThermWin(void);

Boolean alwaysUpdate(void) { return TRUE;}

// display the thermometer using the colored pict
Boolean DisplayTherm(void);

// always do a yeild to a thread
// may not be the windows thread of course
McoStatus updateWindowData(int changed);

// check to see if a timer event has occured
// at timer event check to see if thermometer needs to be updated, 
// an error has occured, or if the thread is finished
Boolean TimerEventPresent(void);

// draw the window
McoStatus DrawWindow(void);	

// update the window
McoStatus UpdateWindow(void);

// get rid of the window's thread
McoStatus 	KillThread(void);

// the thermometer event handler
// checks for timer events and cancel
McoStatus	DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);

};

#endif