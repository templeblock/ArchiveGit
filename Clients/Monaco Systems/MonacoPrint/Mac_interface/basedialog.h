#ifndef BASE_DIALOG_H
#define BASE_DIALOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcostat.h"
#include "mcomem.h"
#include "mcotypes.h"


//design of this class is close related to the event loop
//so interface must be compatible with event interface.
//user should have minimal effect to understand and use this class.
//virtual functions are really intended for user to subclassing.

class BaseDialog{

private:
protected:
	static	short	dialogCounter;	// counter for the stacker
	static 	Rect	screensize;		// screen size( menubar is not included )
	
#if USESROUTINEDESCRIPTORS
	static 	RoutineDescriptor _rd;	
	static 	RoutineDescriptor _sd;	
#endif

	DialogPtr	dialogptr;		
	DialogTHndl dialoghandle;	
	int16		dialogkind;
	DialogRecord	dialogrec;

public:
	BaseDialog(void);	// constructor
	~BaseDialog(void);	// destructor
	
	//findScreenSize should be called by main program before 
	//using this class
	static	void	findScreenSize(void);
	static 	void	createDescriptor(void);

	//in order to used in different kind of windows,
	//method setWindow is use to set to different kinds of window
	virtual McoStatus setDialog(int32);
	
	//how many dialogs
	short	getDialogNumber(void);
	
	//get dialog kind
	long 	getDialogKind(void);
	
	//set dialog kind
	void	setDialogKind(long kind);

	//close the dialog, destroy the dialog
	virtual void	close(void);

	//get size of dialog rect
	virtual	void	getDialogRect(Rect *theRect, Boolean entireWindow);
	
	// frame the button 1, usually the ok button
	virtual	void 	frame_button(void);

	// Added by James, usually a large user item
	virtual void threeD_box(int item);
	
};

void Disable(DialogPtr inDialog, short inItemNum);
void Enable(DialogPtr inDialog, short inItemNum);

#endif //MCO_DIALOG_H