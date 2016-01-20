#include "mldialog.h"


MlDialog::MlDialog(void)
{
;
}


MlDialog::~MlDialog(void)
{
	//BaseDialog:~BaseDialog();
}

int16	MlDialog::hit(EventRecord * event)
{
	// Subclasses must do something here
	return -1;
}

//Update a window.
//The usual thing to do here is to call the Draw()
void	MlDialog::update(void)
{
	BeginUpdate(dialogptr);
	UpdateDialog(dialogptr, dialogptr->visRgn);
	EndUpdate(dialogptr);
	hilite();
}


void	MlDialog::activate(void)
{
	//DrawDialog(dialogptr);
	BeginUpdate(dialogptr);
	UpdateDialog(dialogptr, dialogptr->visRgn);
	EndUpdate(dialogptr);	
	hilite();
}


void	MlDialog::drag(EventRecord* event)
{
	Rect r = qd.screenBits.bounds;
	DragWindow(dialogptr, event->where, &r);
}


void  MlDialog::hilite(void)
{
	//sub class should support this
}	

void	MlDialog::show(void)
{
//	BringToFront((WindowPtr)dialogptr);
	ShowWindow((WindowPtr)dialogptr);
	SelectWindow((WindowPtr)dialogptr);	
}


void	MlDialog::hide(void)
{
	HideWindow((WindowPtr)dialogptr);
}

//subclass should provide member function here
int16	MlDialog::trackClose (Point where)
{
	if (TrackGoAway(dialogptr, where))
		close();
		
	return -1;	
}


void MlDialog::getDialogTitle (Str32 theTitle)
{
	GetWTitle(dialogptr, theTitle);
}


void MlDialog::setDialogTitle (Str32 theTitle)
{
	SetWTitle(dialogptr, theTitle);
}


/****
 * RefreshWindow
 *
 *		Force a window to be redrawn on the next udpate event.
 *		The Boolean parameter determines whether the scroll bars
 *		should be redrawn or not.
 *
 ****/

void MlDialog::refreshWindow (Boolean wholeWindow)
{
	InvalRect(&(dialogptr->portRect));
}


