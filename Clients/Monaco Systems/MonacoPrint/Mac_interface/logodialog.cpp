#include "logodialog.h"

LogoDialog::LogoDialog(void)
{
setDialog(Logo_Dialog);
}	


int16	LogoDialog::hit(void)
{	
	GrafPtr		oldport;
	short		iType;
	Handle		iHandle;
	Rect 		iRect;	
	Str255		str;
	
	GetPort(&oldport);
	SetPort(dialogptr);	
	ShowWindow(dialogptr);
	
	//I don't know what the hell I have to do the following,
	//It took me 6 hours to try to find this
/*	GetDItem (dialogptr, 1, &iType, &iHandle, &iRect);
	DrawPicture((PicHandle)iHandle,&iRect);
	GetDItem (dialogptr, 2, &iType, &iHandle, &iRect);
	GetIText(iHandle, str);
	SetIText(iHandle, str);
	GetDItem (dialogptr, 3, &iType, &iHandle, &iRect);
	GetIText(iHandle, str);
	SetIText(iHandle, str);
*/
		
	ModalDialog (nil, &iType);	
		
	SetPort(oldport);
	return 1; //ok
}



void LogoDialog::setMessage(StringPtr outext)
{
	memcpy(message_str, outext, 256);
}	


