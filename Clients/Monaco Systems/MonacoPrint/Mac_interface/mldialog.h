#ifndef ML_DIALOG_H
#define ML_DIALOG_H

#include "basedialog.h"
#include "mcostat.h"
#include "mcomem.h"
#include "mcotypes.h"


#define	MCODIALOGKIND	'MCO'			


//design of this class is close related to the event loop
//so interface must be compatible with event interface.
//user should have minimal effect to understand and use this class.
//virtual functions are really intended for user to subclassing.

class MlDialog:public BaseDialog{

private:
protected:

public:
	MlDialog(void);	// constructor
	~MlDialog(void);	// destructor
	

	//abstract function, should be filled by subclass
	//hit means mouse select contents area
	virtual int16	hit(EventRecord *);

	//get and set dialog title
	virtual	void	getDialogTitle (Str255 theTitle);
	virtual	void	setDialogTitle (Str255 theTitle);
	
	//hide the dialog
	virtual void	hide(void);

	//drag the dialog
	virtual void	drag(EventRecord* event);

	//mouse select close box, close the dialog
	virtual int16	trackClose(Point where);
	
	//update the contents part of dialog
	virtual void	update(void);
	virtual void 	activate(void);
	
	//dim or undim buttons, subclass must implement this 	
	virtual void  hilite(void);
	
	//show dialog after hiding
	virtual void	show(void);
	
	//redraw the window
	virtual	void	refreshWindow(Boolean entireWindow);
	
};

#endif //ML_DIALOG_H