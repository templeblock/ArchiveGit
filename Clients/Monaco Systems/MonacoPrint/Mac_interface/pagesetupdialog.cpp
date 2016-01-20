#include "pagesetupdialog.h"

#define Pagesetup_Dialog		(2003)
#define Usletter				(5)
#define Tabloid					(6)


PagesetupDialog::PagesetupDialog(void)
{
	short		iType;
	Handle		iHandle;
	Rect 		iRect;	

	setDialog(Pagesetup_Dialog);
	frame_button();	
	
	_which = 5;
	GetDItem (dialogptr, _which, &iType, &iHandle, &iRect);
	SetCtlValue((ControlHandle)iHandle,1);
}	


int16	PagesetupDialog::hit(void)
{	
	short		item;	

	BringToFront(dialogptr);
	ShowWindow(dialogptr);

	do{
		ModalDialog(NULL, &item);
		switch(item){
			case Usletter:	// toggle boxes
			case Tabloid:
			_Setradiobutton(item);
			break;
			
	    	case 3:
	    	break;
	    	
	    	case 1:
			break;	    	
	    }	
	 } while( item != 1 && item != 3);	
	
	return item;	    	
}


void	PagesetupDialog::_Setradiobutton(short item)
{

	if( _which != item ){
		_Setradiostate(_which, 0);		
	
		_which = item;
		_Setradiostate(_which, 1);		
	}
}


void	PagesetupDialog::_Setradiostate(short item, short value)
{
	short		iType;
	Handle		iHandle;
	Rect 		iRect;	

	GetDItem (dialogptr, item, &iType, (Handle*)&iHandle, &iRect);
	SetCtlValue((ControlHandle)iHandle,value);
/*	
	GetDItem (dialogptr, Sep_Off, &iType, (Handle*)&iHandle, &iRect);
	SetCtlValue(iHandle,!GetCtlValue(iHandle));
*/	
}


short	PagesetupDialog::getchoice(void)
{
	short item;
	
	switch(_which){
		case (Usletter):
			item = 1;
			break;
			
		case (Tabloid):
			item = 2;
			break;
			
		default:
			item = 1;
			break;
	}				

	return item;
}


