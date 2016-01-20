//////////////////////////////////////////////////////////////////////////////
//  printdocwindow.h														//
//	The main dialog that has tabs											//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_PRINTDOCWINDOW
#define IN_PRINTDOCWINDOW

#include "mcostat.h"
#include "tabbedwindow.h"
#include "profiledoc.h"
#include "printertype.h"

class PrintDocWindow:public TabbedWindow {
private:
protected:

	ProfileDoc	*doc;
	PrinterType printers;
	
	int 		doingProcess;

	
public:

	PrintDocWindow(ProfileDoc *dc,int winnum);
	
	~PrintDocWindow(void);
	
	McoStatus loadLastPrinter(void);

	McoStatus saveLastPrinter(void);

	McoStatus setParameters(void);

	McoStatus getParameters(void);

	// open the tab
	McoStatus openTab(void);
	
	Boolean alwaysUpdate(void) { return TRUE;}
	
	// update the data within a window and any tabs within the window
	McoStatus updateWindowData(int changed);
	
	// draw the tabs
	McoStatus DrawTabs();
	
	McoStatus UpdateWindow(void);
	
	// determine if event belongs to window (used for slider)
	Boolean isMyObject(Point where,short *item);
	
	Boolean TimerEventPresent(void);
	
	McoStatus makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn);
	
	McoStatus doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype);
	
	// handle keyboard events
	Boolean 	KeyFilter(EventRecord *theEvent,short *itemHit );
	
	// The event handler for the main menu
	McoStatus	DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);
};

#endif