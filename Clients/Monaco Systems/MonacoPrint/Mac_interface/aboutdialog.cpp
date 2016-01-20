#include "aboutdialog.h"

AboutDialog::AboutDialog(void)
{
	setDialog(About_Dialog);
	frame_button();
}	

int16	AboutDialog::hit(void)
{	
	GrafPtr		saveport;
	short		item;

	GetPort(&saveport);
	SetPort(dialogptr);
	BringToFront(dialogptr);
	TextFont(geneva);
	TextFace(0);
	TextSize(10);

	ShowWindow(dialogptr);

	ModalDialog(NULL, &item);	
	SetPort(saveport);
	return item;	    	
}
